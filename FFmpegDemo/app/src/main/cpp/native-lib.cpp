#include <jni.h>
extern "C" {
  #include "libavcodec/avcodec.h"
}
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_chadlin_ffmpegdemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}