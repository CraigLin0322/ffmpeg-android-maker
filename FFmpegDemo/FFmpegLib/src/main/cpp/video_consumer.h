#ifndef __VIDEO_CONSUMER_H__
#define __VIDEO_CONSUMER_H__

#include "media_consumer.h"

class VideoConsumer : public MediaConsumer {
protected:
    const char *TAG = "VideoConsumer";
public:
    int decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context,
                     int stream_index) const override;

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface) const override;

    void seekTo(JNIEnv *env, jlong position) const override;

    void pause(JNIEnv *env) const override;

    void resume(JNIEnv *env) const override;

    void releaseResource() const override;

    void initResource() const override;
    ~VideoConsumer() {

    }

};

#endif // __VIDEO_CONSUMER_H__
