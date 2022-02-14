#ifndef __MEDIA_PRODUCER_H__
#define __MEDIA_PRODUCER_H__

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

#include "video_player_listener.h"
#include "ffmpeg_define.h"

enum VideoState {NOT_STARTED,PLAYING,PAUSED};
class MediaProducerSingleton {
public:
    static MediaProducerSingleton &Instance() {
        // Since it's a static variable, if the class has already been created,
        // it won't be created again.
        // And it **is** thread-safe in C++11.
        static MediaProducerSingleton myInstance;

        // Return a reference to our instance.
        return myInstance;
    }

    // delete copy and move constructors and assign operators
    MediaProducerSingleton(MediaProducerSingleton const &) = delete;             // Copy construct
    MediaProducerSingleton(MediaProducerSingleton &&) = delete;                  // Move construct
    MediaProducerSingleton &operator=(MediaProducerSingleton const &) = delete;  // Copy assign
    MediaProducerSingleton &operator=(MediaProducerSingleton &&) = delete;      // Move assign

    // Any other public methods.
    int play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

protected:
    VideoState videoState;
    MediaProducerSingleton() {
        // Constructor code goes here.
    }

    ~MediaProducerSingleton() {
        // Destructor code goes here.
    }

    // And any other protected methods.
    void reset();
};

#endif // __MEDIA_PRODUCER_H__
