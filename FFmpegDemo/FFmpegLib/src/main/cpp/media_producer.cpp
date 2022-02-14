#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <unistd.h>
#include "media_producer.h"



#define TAG "VideoPlayer"
//https://www.jianshu.com/p/c7de148e951c
//https://blog.csdn.net/JohanMan/article/details/83091706

void MediaProducerSingleton::pause(JNIEnv *env) {
    if (videoState == VideoState::PLAYING) {

    }
}

void MediaProducerSingleton::resume(JNIEnv *env) {
    if (videoState == VideoState::PAUSED) {

    }
}

void MediaProducerSingleton::seekTo(JNIEnv *env, jlong position) {
    //Core implementation of seeking to position
    //av_seek_frame(formatContext, -1, seek, AVSEEK_FLAG_BACKWARD);
    if (videoState == VideoState::NOT_STARTED) {
        return;
    }
}

void MediaProducerSingleton::reset() {
    //TODO release resource while error happening
    videoState = VideoState::NOT_STARTED;
}

int MediaProducerSingleton::play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath,
                                 jobject surface) {
    //TODO release resource and reset flags when error happens.

    if (videoState != VideoState::NOT_STARTED) {
        listener->onError(VIDEO_DUP_PLAY);
        return VIDEO_STATUS_FAILURE
    }
    listener->onStart();
    videoState = VideoState::PLAYING;
    int result;
    const char *path = env->GetStringUTFChars(javaPath, 0);
    // Register components
    av_register_all();
    // init AVFormatContext
    AVFormatContext *format_context = avformat_alloc_context();
    // Open video files
    result = avformat_open_input(&format_context, path, NULL, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not open video file");
        listener->onError(VIDEO_ERROR_OPEN);
        return VIDEO_STATUS_FAILURE
    }
    // Query video stream info
    result = avformat_find_stream_info(format_context, NULL);
    if (result < 0) {
        LOGE(TAG, " : Can not find video file stream info");
        listener->onError(VIDEO_ERROR_FIND_VIDEO_STREAM_INFO);
        return VIDEO_STATUS_FAILURE
    }
    // Find decoder
    int video_stream_index = -1;
    int audio_stream_index = -1;
    for (int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        } else if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
        }
    }
    if (video_stream_index == -1) {
        LOGE(TAG, " : Can not find video stream");
        listener->onError(VIDEO_ERROR_FIND_VIDEO_STREAM);
        return VIDEO_STATUS_FAILURE
    }
    // init decoder context
    AVCodecContext *video_codec_context = avcodec_alloc_context3(NULL);

    //TODO separate flow of audio and video here
    avcodec_parameters_to_context(video_codec_context,
                                  format_context->streams[video_stream_index]->codecpar);
    AVCodec *video_codec = avcodec_find_decoder(video_codec_context->codec_id);
    if (video_codec == NULL) {
        LOGE(TAG, " : Can not find video codec");
        listener->onError(VIDEO_ERROR_FIND_DECODER);
        return VIDEO_STATUS_FAILURE
    }
    // Open video decoder
    result = avcodec_open2(video_codec_context, video_codec, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not find video stream");
        listener->onError(VIDEO_ERROR_OPEN_DECODER);
        return VIDEO_STATUS_FAILURE
    }
    int videoWidth = video_codec_context->width;
    int videoHeight = video_codec_context->height;
    // Init ANativeWindow
    ANativeWindow *native_window = ANativeWindow_fromSurface(env, surface);
    if (native_window == NULL) {
        LOGE(TAG, " : Can not create native window");
        listener->onError(VIDEO_ERROR_CREATE_NATIVE_WINDOW);
        return VIDEO_STATUS_FAILURE
    }

    result = ANativeWindow_setBuffersGeometry(native_window, videoWidth, videoHeight,
                                              WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        LOGE(TAG, " : Can not set native window buffer");
        listener->onError(VIDEO_ERROR_SET_NATIVE_WINDOW_BUFFER);
        ANativeWindow_release(native_window);
        return VIDEO_STATUS_FAILURE
    }
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
    AVRational time_base = format_context->streams[video_stream_index]->time_base;

    // 开始读取帧
    while (av_read_frame(format_context, packet) >= 0) {
        // 匹配视频流
        if (packet->stream_index == video_stream_index) {
            // 解码
            result = avcodec_send_packet(video_codec_context, packet);
            if (result < 0 && result != AVERROR(EAGAIN) && result != AVERROR_EOF) {
                LOGE(TAG, " : codec step 1 fail");
                listener->onError(VIDEO_ERROR_DECODE_FAIL);
                return VIDEO_STATUS_FAILURE
            }
            result = avcodec_receive_frame(video_codec_context, frame);
            if (result < 0 && result != AVERROR_EOF) {
                LOGE(TAG, " : codec step 2 fail");
                listener->onError(VIDEO_ERROR_RECEIVE_FRAME);
                return VIDEO_STATUS_FAILURE
            }
            double timestamp = frame->best_effort_timestamp * av_q2d(time_base);
            listener->onProgress(duration, timestamp);
            // 数据格式转换
            result = sws_scale(
                    data_convert_context,
                    (const uint8_t *const *) frame->data, frame->linesize,
                    0, videoHeight,
                    rgba_frame->data, rgba_frame->linesize);
            if (result <= 0) {
                LOGE("Player Error ", ": data convert fail");
                listener->onError(VIDEO_ERROR_CONVERT_DATA);
                return VIDEO_STATUS_FAILURE
            }
            // 播放
            result = ANativeWindow_lock(native_window, &window_buffer, NULL);
            if (result < 0) {
                listener->onError(VIDEO_ERROR_LOCK_NATIVE_WINDOW);
                LOGE(TAG, " : Can not lock native window");
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
        }
        // 释放 packet 引用
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
    env->ReleaseStringUTFChars(javaPath, path);

    reset();

    listener->onStop();

    return VIDEO_STATUS_SUCCESS
}
