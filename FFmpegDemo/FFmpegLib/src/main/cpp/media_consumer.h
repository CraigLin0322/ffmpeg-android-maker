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
#include "ffmpeg_define.h"

#ifdef __cplusplus
}
#endif

#define INVALID_STREAM_INDEX -1

class MediaConsumer {

protected:
    int stream_index = INVALID_STREAM_INDEX;

    virtual int decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context, int stream_index);

    virtual int
    play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath, jobject surface);

    virtual void seekTo(JNIEnv *env, jlong position) = 0;

    virtual void pause(JNIEnv *env) = 0;

    virtual void resume(JNIEnv *env) = 0;

    virtual void releaseResource() = 0;

    virtual void initResource() = 0;
};


#endif // __MEDIA_CONSUMER_H__
