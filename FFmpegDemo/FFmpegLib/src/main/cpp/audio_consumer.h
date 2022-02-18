#ifndef __AUDIO_CONSUMER_H__
#define __AUDIO_CONSUMER_H__

#include "media_consumer.h"
#include "jni.h"
#include "cstring"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

namespace AudioConsumer {
    int decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context,
                     int stream_index);

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

    void releaseResource();

    void initResource();

    void initBufferQueue(int rate, int channel, int bitsPerSample);

    void bpPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

}

#endif // __AUDIO_CONSUMER_H__
