#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "ffmpeg_define.h"

#ifdef __cplusplus
}
#endif

#define TAG "VideoPlayer"
//https://www.jianshu.com/p/c7de148e951c
float play_rate = 1;
long duration = 0;

//https://blog.csdn.net/JohanMan/article/details/83091706
int play(JNIEnv *env, jstring path, jobject surface) {

    const char *nativePath = env->GetStringUTFChars(path, 0);
    //Register all component
    av_register_all();

    // Encapsulate format into context
    AVFormatContext *formatContext = avformat_alloc_context();

    int result = avformat_open_input(&formatContext, nativePath, NULL, NULL);

    if (result < 0) {
        LOGE(TAG, "Fail in opening video files");
        return STATUS_FAILURE;
    }

    int video_stream_index = -1;

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        // Find if this streams meet the video requirement
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }
    }
    if (video_stream_index == -1) {
        LOGE(TAG, "This video files is not supported");
        return STATUS_FAILURE;
    }

    //Initialize video codec context
    AVCodecContext *avCodecContext = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(avCodecContext,
                                  formatContext->streams[video_stream_index]->codecpar);

    //Initialize video codec
    AVCodec *video_codec = avcodec_find_decoder(avCodecContext->codec_id);

    if (video_codec == NULL) {
        LOGE(TAG, "Error in finding video codec");
        return STATUS_FAILURE;
    }

    result = avcodec_open2(avCodecContext, video_codec, NULL);

    if (result < 0) {
        LOGE(TAG, "Error in opening codec");
        return STATUS_FAILURE;
    }

    int videoWidth = avCodecContext->width;
    int videoHeight = avCodecContext->height;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    if (nativeWindow == NULL) {
        LOGE(TAG, "Fail in creating ANativeWindow");
        return STATUS_FAILURE;
    }

    result = ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                              WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        LOGE(TAG, "Fail in setting Buffer Geometry for ANativeWindow");
        ANativeWindow_release(nativeWindow);
        return STATUS_FAILURE;
    }

    ANativeWindow_Buffer windowBuffer;

    AVPacket *packet = av_packet_alloc();

    AVFrame *frame = av_frame_alloc();

    AVFrame *rgba_frame = av_frame_alloc();

    int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoWidth, videoHeight, 1);

    uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size * sizeof(uint8_t));

    struct SwsContext *data_convert_context = sws_getContext(videoWidth, videoHeight,
                                                             avCodecContext->pix_fmt, videoWidth,
                                                             videoHeight,
                                                             AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL,
                                                             NULL, NULL);
    //Read frame
    while (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == video_stream_index) {
            //decode
            result = avcodec_send_packet(avCodecContext, packet);
            if (result < 0 && result != AVERROR(EAGAIN) && result != AVERROR_EOF) {
                LOGE(TAG, "Send packet fails");
                return STATUS_FAILURE;
            }
            result = avcodec_receive_frame(avCodecContext, frame);
            if (result < 0 && result != AVERROR_EOF) {
                LOGE(TAG, "Error in receiving frame");
                return STATUS_FAILURE;
            }
            // Convert data format
            result = sws_scale(data_convert_context, (const uint8_t *const *) frame->data,
                               frame->linesize,
                               0, videoHeight, rgba_frame->data, rgba_frame->linesize);
            if (result <= 0) {
                LOGE(TAG, "Error in converting data");
                return STATUS_FAILURE;
            }

            result = ANativeWindow_lock(nativeWindow, &windowBuffer, NULL);

            if (result < 0) {
                LOGE(TAG, "Cannot lock native window");
            } else {
                uint8_t *bits = (uint8_t *) windowBuffer.bits;
                for (int h = 0; h < videoHeight; ++h) {
                    memcpy(bits + h * windowBuffer.stride * 4,
                           out_buffer + h * rgba_frame->linesize[0],
                           rgba_frame->linesize[0]);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }

        av_packet_unref(packet);
    }

    sws_freeContext(data_convert_context);

    av_free(out_buffer);

    av_frame_free(&rgba_frame);

    av_frame_free(&frame);

    av_packet_free(&packet);

    ANativeWindow_release(nativeWindow);

    avcodec_close(avCodecContext);

    avformat_close_input(&formatContext);

    env->ReleaseStringUTFChars(path, nativePath);

    return STATUS_SUCCESS;
};