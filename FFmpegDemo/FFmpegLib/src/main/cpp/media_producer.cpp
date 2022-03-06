#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <unistd.h>
#include "media_producer.h"
#include "video_consumer.h"
#include "audio_consumer.h"


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

int MediaProducerSingleton::play(VideoPlayListener *listener, const std::string javaPath,
                                 ANativeWindow * nativeWindow) {
    //TODO release resource and reset flags when error happens.
    std::lock_guard<std::mutex> lock(mutex);
    if (videoState != VideoState::NOT_STARTED) {
        listener->onError(VIDEO_DUP_PLAY);
        return VIDEO_STATUS_FAILURE
    }
    listener->onStart();
    videoState = VideoState::PLAYING;
    int result;
    // Register components
    av_register_all();
    // init AVFormatContext
    AVFormatContext *format_context = avformat_alloc_context();
    // Open video files
    result = avformat_open_input(&format_context, javaPath.c_str(), NULL, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not open video file");
        listener->onError(VIDEO_ERROR_OPEN);
        return VIDEO_STATUS_FAILURE
    }
    // Query media stream info
    result = avformat_find_stream_info(format_context, NULL);
    if (result < 0) {
        LOGE(TAG, " : Can not find video file stream info");
        listener->onError(VIDEO_ERROR_FIND_VIDEO_STREAM_INFO);
        return VIDEO_STATUS_FAILURE
    }
    LOGE(TAG, " wwwwwww1");

    // Find decoder index
    for (int i = 0; i < format_context->nb_streams; i++) {
        if (format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        } else if (format_context->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
        }
    }
    if (video_stream_index == -1) {
        LOGE(TAG, " : Can not find video stream");
        listener->onError(VIDEO_ERROR_FIND_VIDEO_STREAM);
        return VIDEO_ERROR_FIND_VIDEO_STREAM;
    }

    if (audio_stream_index == -1) {
        LOGE(TAG, " : Can not find audio stream");
        listener->onError(VIDEO_ERROR_FIND_VIDEO_STREAM);
        return VIDEO_ERROR_FIND_VIDEO_STREAM;
    }
    LOGE(TAG, " wwwwwww2");
    const int succeed = VIDEO_STATUS_SUCCESS;

    int status;
    auto *mediaContext = new MediaContext;
    mediaContext->formatContext = format_context;
    mediaContext->stream_video_index = video_stream_index;
    mediaContext->stream_audio_index = audio_stream_index;
    mediaContext->nativeWindow = nativeWindow;

    AVPacket *packet = av_packet_alloc();
    status = videoConsumer->initResource(mediaContext);
    if (succeed != status) {
        return status;
    }
    status = videoConsumer->play();
    while (videoState == VideoState::PLAYING) {
        if (av_read_frame(format_context, packet) == 0) {
            if (packet->stream_index == audio_stream_index) {
                audioConsumer->put(packet);
            } else if (packet->stream_index == video_stream_index) {
                videoConsumer->put(packet);
            }

            //TODO consider video is over but audio hasn't yet.
        } else{

        }
        av_packet_unref(packet);
    }

    av_free_packet(packet);
    avformat_free_context(format_context);
//    status = audioConsumer->initResource(format_context, audio_stream_index);
//    if (succeed != status) {
//        return status;
//    }
//    status = audioConsumer->play();
    if (succeed != status) {
        return status;
    }

//    avformat_close_input(&format_context);
//
    reset();

    listener->onStop();

    return VIDEO_STATUS_SUCCESS;

}