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
    video_codec_context = format_context->streams[video_stream_index]->codec;
    audio_codec_context = format_context->streams[video_stream_index]->codec;

    video_codec = avcodec_find_decoder(video_codec_context->codec_id);
    if (video_codec == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }

    if (audio_codec == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }
    // Open video decoder
    result = avcodec_open2(video_codec_context, video_codec, NULL);
    audio_codec = avcodec_find_decoder(audio_codec_context->codec_id);
    if (result < 0) {
        LOGE(TAG, ": Can not find video stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }
    videoWidth = video_codec_context->width;
    videoHeight = video_codec_context->height;
    // Init ANativeWindow
    native_window = ANativeWindow_fromSurface(env, surface);
    if (native_window == NULL) {
        LOGE(TAG, " : Can not create native window");
        return VIDEO_ERROR_CREATE_NATIVE_WINDOW;
    }

    //Init Audio
    //TODO


    avformat_close_input(&format_context);
    env->ReleaseStringUTFChars(javaPath, path);

    reset();

    listener->onStop();

    return VIDEO_STATUS_SUCCESS
}

int initInputFormatContext(MediaProducerSingleton *singleton, const char *javaPath) {

}

int initCodecContext(MediaProducerSingleton *singleton) {

}
