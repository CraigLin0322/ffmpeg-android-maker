#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

int play(JNIEnv *env, jstring path, jobject surface);

class VideoPlayListener {
//https://blog.csdn.net/ywl5320/article/details/78739211
public:
    JavaVM *vm;
    JNIEnv *env;
    jobject jobj;
    jmethodID jmethodId;

public:
    VideoPlayListener(JavaVM *vm, JNIEnv *env, jobject obj);

    ~VideoPlayListener();

    void onError(int type, int code, const char *msg);

};

#endif // __VIDEO_PLAYER_H__
