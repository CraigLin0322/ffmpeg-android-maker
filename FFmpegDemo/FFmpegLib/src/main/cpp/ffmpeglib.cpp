#include <jni.h>
#include <string>
#include "video_player.h"
#include "video_player_listener.h"
#include "ffmpeg_define.h"

extern "C" {
#include "libavcodec/avcodec.h"
}
#define TAG "LOAD_NATIVE"
static const char *const JAVA_HOST_CLASS = "com/chadlin/ffmpeglib/FFmpegVideoManager";

static jclass nativeClazz;
JavaVM *jvm;
VideoPlayListener *playListener;

JNIEXPORT jstring JNICALL pinService(JNIEnv *env, jobject /* this */) {
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jboolean JNICALL playLocalVideo(JNIEnv *env, jobject thiz,
                                          jstring local_path, jobject surface,
                                          jobject callback) {
    if (playListener == nullptr) {
        playListener = new VideoPlayListener(jvm, env, env->NewGlobalRef(callback));
    }
    return play(env, playListener, local_path, surface, 0) >= 0;
}

JNIEXPORT jint JNICALL init(JNIEnv *env, jobject thiz, jboolean runInThread) {
    std::string hello = avcodec_configuration();
    if (hello.empty()) {
        return INITIALIZE_FAIL
    }
    return INITIALIZE_SUCCEED
}

JNIEXPORT void JNICALL deInit(JNIEnv *env, jobject thiz) {
    if (playListener != nullptr) {
        free(playListener);
        delete playListener;
    }
}


JNIEXPORT void JNICALL pause(JNIEnv *env, jobject thiz) {
    // TODO: implement pauseVideo()
}

JNIEXPORT void JNICALL resume(JNIEnv *env, jobject thiz) {
    // TODO: implement pauseVideo()
}

JNIEXPORT void JNICALL setProgress(JNIEnv *env, jobject thiz, jlong progress) {
    // TODO: implement pauseVideo()
}

static JNINativeMethod gMethods[] = {
        {"testConnection",     "()Ljava/lang/String;",                                                                   (jstring *) pinService},
        {"playLocalVideo",     "(Ljava/lang/String;Landroid/view/Surface;Lcom/chadlin/ffmpeglib/VideoPlayerCallback;)Z", (jboolean *) playLocalVideo},
        {"initializeResource", "(Z)I",                                                                                   (jint *) init},
        {"releaseResource",    "()V",                                                                                    (void *) deInit},
        {""},
        {"pauseVideo","()V",(void *) pause},
        {"resumeVideo","()V",(void *) resume},
        {"setVideoProgress","(J)V",(void *) setProgress}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    jvm = vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }

    nativeClazz = env->FindClass(JAVA_HOST_CLASS);

    if (!nativeClazz) {
        LOGE(TAG, "Error on find class while entering JNI_OnLoad");
        return result;
    }

    if (env->RegisterNatives(nativeClazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0]))) {
        LOGE(TAG, "Error on register natives while entering JNI_OnLoad");
        return result;
    }

    return JNI_VERSION_1_4;
}

