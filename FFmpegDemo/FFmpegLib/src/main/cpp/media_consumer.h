#ifndef __MEDIA_CONSUMER_H__
#define __MEDIA_CONSUMER_H__

#include <jni.h>
#include "video_player_listener.h"
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <unistd.h>
#include "ffmpeg_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

#ifdef __cplusplus
}
#endif

struct MediaContext {
    ANativeWindow * nativeWindow;
    AVFormatContext * formatContext;
    int stream_video_index;
    int stream_audio_index;
};
class MediaConsumer {
public:
    MediaConsumer(){

    }

    ~MediaConsumer(){

    }

    int put(AVPacket *packet);

    int get(AVPacket *packet);

    int decodeStream() ;

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

    void releaseResource();

    int initResource(MediaContext *mediaContext);
};

#endif // __MEDIA_CONSUMER_H__
