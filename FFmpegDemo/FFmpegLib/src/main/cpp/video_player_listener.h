#ifndef __VIDEO_PLAYER_LISTENER_H__
#define __VIDEO_PLAYER_LISTENER_H__

#include <jni.h>

#define THREAD_TYPE_SYNC  0
#define THREAD_TYPE_ASYNC 1

class VideoPlayListener {
//https://blog.csdn.net/ywl5320/article/details/78739211
public:
    JavaVM *vm;
    JNIEnv *env;
    jobject jobj;
    jmethodID jmethodErrorId;
    jmethodID jmethodStartId;
    jmethodID jmethodProgressId;
    jmethodID jmethodEndId;

public:
    VideoPlayListener(JavaVM *vm, JNIEnv *env, jobject obj, jboolean runOnThread);

    ~VideoPlayListener();

    void onError(int code) const;

    void onStart() const;

    void onStop() const;

    void onProgress(int total, int progress) const;

};


#endif // __VIDEO_PLAYER_LISTENER_H__
