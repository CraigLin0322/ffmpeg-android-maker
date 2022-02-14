#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

#include "video_player_listener.h"
#include "ffmpeg_define.h"
enum VideoState {NOT_STARTED,PLAYING,PAUSED};
class VideoPlayerSingleton {
public:
    static VideoPlayerSingleton &Instance() {
        // Since it's a static variable, if the class has already been created,
        // it won't be created again.
        // And it **is** thread-safe in C++11.
        static VideoPlayerSingleton myInstance;

        // Return a reference to our instance.
        return myInstance;
    }

    // delete copy and move constructors and assign operators
    VideoPlayerSingleton(VideoPlayerSingleton const &) = delete;             // Copy construct
    VideoPlayerSingleton(VideoPlayerSingleton &&) = delete;                  // Move construct
    VideoPlayerSingleton &operator=(VideoPlayerSingleton const &) = delete;  // Copy assign
    VideoPlayerSingleton &operator=(VideoPlayerSingleton &&) = delete;      // Move assign

    // Any other public methods.
    int play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

protected:
    VideoState videoState;
    VideoPlayerSingleton() {
        // Constructor code goes here.
    }

    ~VideoPlayerSingleton() {
        // Destructor code goes here.
    }

    // And any other protected methods.
    void reset();
};

#endif // __VIDEO_PLAYER_H__
