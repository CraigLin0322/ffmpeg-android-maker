#ifndef __AUDIO_CONSUMER_H__
#define __AUDIO_CONSUMER_H__

#include "media_consumer.h"
#include "jni.h"
#include "cstring"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

namespace AudioConsumer {
    int decodeStream();

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

    void releaseResource();

    int initResource(AVFormatContext *formatContext, int index);

}

#endif // __AUDIO_CONSUMER_H__
