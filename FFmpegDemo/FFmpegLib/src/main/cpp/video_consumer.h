#ifndef __VIDEO_CONSUMER_H__
#define __VIDEO_CONSUMER_H__

#include "media_consumer.h"

class VideoConsumer {
public:

    int videoHeight;
    int videoWidth;
    ANativeWindow *native_window;
    AVCodecContext *video_codec_context;
    AVFormatContext *format_context;
    AVCodec *video_codec;
    int playRate = 1;
    int video_stream_index = -1;

    int decodeStream();

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

    void releaseResource();

    int
    initResource(AVFormatContext *format_context, int stream_index, ANativeWindow *window);
};

#endif // __VIDEO_CONSUMER_H__
