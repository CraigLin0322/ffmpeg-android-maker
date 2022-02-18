#ifndef __VIDEO_CONSUMER_H__
#define __VIDEO_CONSUMER_H__

#include "media_consumer.h"

namespace VideoConsumer {
    int decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context,
                     int stream_index) ;

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface) ;

    void seekTo(JNIEnv *env, jlong position) ;

    void pause(JNIEnv *env)  ;

    void resume(JNIEnv *env)  ;

    void releaseResource() ;

    void initResource() ;
}

#endif // __VIDEO_CONSUMER_H__
