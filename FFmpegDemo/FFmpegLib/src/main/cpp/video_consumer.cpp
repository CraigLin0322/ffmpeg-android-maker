#include "video_consumer.h"

using namespace VideoConsumer;
static const char *TAG = "VideoConsumer";

int videoHeight;
int videoWidth;
ANativeWindow *native_window;
AVCodecContext *video_codec_context;
AVFormatContext *format_context;
AVCodec *video_codec;
int playRate = 1;
int video_stream_index = -1;

int VideoConsumer::decodeStream() {
    int result;

    ANativeWindow_Buffer window_buffer;
    // 声明数据容器 有3个
    // R5 解码前数据容器 Packet 编码数据
    AVPacket *packet = av_packet_alloc();
    // R6 解码后数据容器 Frame 像素数据 不能直接播放像素数据 还要转换
    AVFrame *frame = av_frame_alloc();
    // R7 转换后数据容器 这里面的数据可以用于播放
    AVFrame *rgba_frame = av_frame_alloc();
    // 数据格式转换准备
    // 输出 Buffer
    int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoWidth, videoHeight, 1);
    // R8 申请 Buffer 内存
    uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(rgba_frame->data, rgba_frame->linesize, out_buffer, AV_PIX_FMT_RGBA,
                         videoWidth, videoHeight, 1);
    // R9 数据格式转换上下文
    struct SwsContext *data_convert_context = sws_getContext(
            videoWidth, videoHeight, video_codec_context->pix_fmt,
            videoWidth, videoHeight, AV_PIX_FMT_RGBA,
            SWS_BICUBIC, NULL, NULL, NULL);

    //Get duration about Video
    long duration = 0;
    if (format_context->duration != AV_NOPTS_VALUE) {
        duration = format_context->duration / AV_TIME_BASE;
    }
    AVRational time_base = video_codec_context->time_base;
    double timestamp = 0l;
    int finish = 0;
    // Read frame
    while (av_read_frame(format_context, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {
            avcodec_decode_video2(video_codec_context, frame, &finish,
                                  packet);
            if (finish) {
                timestamp = frame->best_effort_timestamp * av_q2d(time_base);
//            listener->onProgress(duration, timestamp);
                result = sws_scale(
                        data_convert_context,
                        (const uint8_t *const *) frame->data, frame->linesize,
                        0, videoHeight,
                        rgba_frame->data, rgba_frame->linesize);
                if (result <= 0) {
                    LOGE("Player Error ", ": data convert fail");
                    return VIDEO_ERROR_CONVERT_DATA;
                }
                // play
                result = ANativeWindow_lock(native_window, &window_buffer, NULL);
                if (result < 0) {
                    LOGE(TAG, " : Can not lock native window");
                    return VIDEO_ERROR_LOCK_NATIVE_WINDOW;
                } else {
                    // 将图像绘制到界面上
                    // 注意 : 这里 rgba_frame 一行的像素和 window_buffer 一行的像素长度可能不一致
                    // 需要转换好 否则可能花屏
                    uint8_t *bits = (uint8_t *) window_buffer.bits;
                    for (int h = 0; h < videoHeight; h++) {
                        memcpy(bits + h * window_buffer.stride * 4,
                               out_buffer + h * rgba_frame->linesize[0],
                               rgba_frame->linesize[0]);
                    }
                    ANativeWindow_unlockAndPost(native_window);
                }
                //TODO implment fast forward/rewind
//                usleep((unsigned long) (1000 * 40 * playRate));
            }
        }
        // release packet reference
        av_packet_unref(packet);
    }
    sws_freeContext(data_convert_context);
    av_free(out_buffer);
    av_frame_free(&rgba_frame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    ANativeWindow_release(native_window);
    avcodec_close(video_codec_context);
    avformat_close_input(&format_context);
    return VIDEO_STATUS_SUCCESS
}

void VideoConsumer::resume(JNIEnv *env) {

}

void VideoConsumer::pause(JNIEnv *env) {

}

int VideoConsumer::initResource(AVFormatContext *formatContext, int index, JNIEnv *env,
                                jobject surface) {
    int result = VIDEO_STATUS_FAILURE;
    format_context = formatContext;
    video_stream_index = index;

    video_codec_context = format_context->streams[video_stream_index]->codec;
    video_codec = avcodec_find_decoder(video_codec_context->codec_id);
    if (video_codec == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }
    result = avcodec_open2(video_codec_context, video_codec, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not find video stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }
    videoHeight = video_codec_context->height;
    videoWidth = video_codec_context->width;

    // Init ANativeWindow
    native_window = ANativeWindow_fromSurface(env, surface);
    if (native_window == NULL) {
        LOGE(TAG, " : Can not create native window");
        return VIDEO_ERROR_CREATE_NATIVE_WINDOW;
    }

    result = ANativeWindow_setBuffersGeometry(native_window, videoWidth, videoHeight,
                                              WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        LOGE(TAG, " : Can not set native window buffer");
        ANativeWindow_release(native_window);
        return VIDEO_ERROR_SET_NATIVE_WINDOW_BUFFER;
    }
    return VIDEO_STATUS_SUCCESS;
}

void VideoConsumer::releaseResource() {

}

int VideoConsumer::play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath,
                        jobject surface) {

}

void VideoConsumer::seekTo(JNIEnv *env, jlong position) {

}
