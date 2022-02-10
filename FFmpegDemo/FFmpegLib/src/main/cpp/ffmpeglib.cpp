#include <jni.h>
#include <string>
#include "video_player.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_chadlin_ffmpeglib_FFmpegVideoManager_testConnection(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_chadlin_ffmpeglib_FFmpegVideoManager_playLocalVideo(JNIEnv *env, jobject thiz,
                                                             jstring local_path, jobject surface) {
    return play(env, local_path, surface) >= 0;
}