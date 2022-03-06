#ifndef __VIDEO_CONSUMER_H__
#define __VIDEO_CONSUMER_H__

#include "media_consumer.h"

class VideoConsumer : public MediaConsumer {
public:

    int videoHeight;
    int videoWidth;
    ANativeWindow *native_window;
    AVCodecContext *video_codec_context;
    AVFormatContext *format_context;
    AVCodec *video_codec;
    int playRate = 1;
    int video_stream_index = -1;

    int play() override;

    void seekTo(JNIEnv *env, jlong position) override;

    void pause(JNIEnv *env) override;

    void resume(JNIEnv *env) override;

    void releaseResource() override;

    int initResource(MediaContext *mediaContext) override;


    VideoConsumer();

    ~VideoConsumer();
};

#endif // __VIDEO_CONSUMER_H__
