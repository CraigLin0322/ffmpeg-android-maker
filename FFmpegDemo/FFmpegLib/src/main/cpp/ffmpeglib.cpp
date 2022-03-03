#include <jni.h>
#include <string>
#include "media_producer.h"
#include "video_player_listener.h"
#include "ffmpeg_define.h"
#include <pthread.h>

extern "C" {
#include "libavcodec/avcodec.h"
}
#define TAG "LOAD_NATIVE"
static const char *const JAVA_HOST_CLASS = "com/chadlin/ffmpeglib/FFmpegVideoManager";

static jclass nativeClazz;
jobject videoCallback;
bool runInThreadNative;
JavaVM *jvm;
VideoPlayListener *playListener;
struct ThreadArgs {
    void *listener;
    std::string path;
    ANativeWindow *nativeWindow;
};

JNIEXPORT jstring JNICALL pinService(JNIEnv *env, jobject /* this */) {
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

void* play(void *args) {
    auto *threadArgs = static_cast<ThreadArgs *>(args);
    auto *videoPlayListener = static_cast<VideoPlayListener *>(threadArgs->listener);
    MediaProducerSingleton::Instance().play(videoPlayListener, threadArgs->path,
                                            threadArgs->nativeWindow);
    pthread_exit((void *) 2);
}


JNIEXPORT jboolean JNICALL playLocalVideo(JNIEnv *env, jobject thiz,
                                          jstring local_path, jobject surface) {
    if (playListener == nullptr) {
        //Note that this env could be different everytime call from java side
        playListener = new VideoPlayListener(jvm, env, videoCallback, runInThreadNative);
    }
    ANativeWindow *newWindow = ANativeWindow_fromSurface(env, surface);
    if (newWindow == nullptr) {
        return 0;
    }
    const char *path = env->GetStringUTFChars(local_path, 0);
    std::string filePath = std::string(path);
    auto *threadArgs = new ThreadArgs;
    threadArgs->listener = playListener;
    threadArgs->path = filePath;
    threadArgs->nativeWindow = newWindow;
    pthread_t pthread1;
    pthread_create(&pthread1, NULL, play, (void *) threadArgs);
    env->ReleaseStringUTFChars(local_path, path);
    return 1;
}

JNIEXPORT jint JNICALL init(JNIEnv *env, jobject thiz, jboolean runInThread, jobject callback) {
    std::string hello = avcodec_configuration();
    if (hello.empty()) {
        return INITIALIZE_FAIL
    }

    videoCallback = env->NewGlobalRef(callback);
    runInThreadNative = runInThread;
    return INITIALIZE_SUCCEED
}

JNIEXPORT void JNICALL deInit(JNIEnv *env, jobject thiz) {
    if (playListener != nullptr) {
        env->DeleteGlobalRef(videoCallback);
        delete playListener;
    }
}


JNIEXPORT void JNICALL videoPause(JNIEnv *env, jobject thiz) {
    MediaProducerSingleton::Instance().pause(env);
}

JNIEXPORT void JNICALL videoResume(JNIEnv *env, jobject thiz) {
    MediaProducerSingleton::Instance().resume(env);
}

JNIEXPORT void JNICALL setProgress(JNIEnv *env, jobject thiz, jlong progress) {
    MediaProducerSingleton::Instance().seekTo(env, progress);
}

static JNINativeMethod gMethods[] = {
        {"testConnection",     "()Ljava/lang/String;",                            (jstring *) pinService},
        {"playLocalVideo",     "(Ljava/lang/String;Landroid/view/Surface;)Z",     (jboolean *) playLocalVideo},
        {"initializeResource", "(ZLcom/chadlin/ffmpeglib/VideoPlayerCallback;)I", (jint *) init},
        {"releaseResource",    "()V",                                             (void *) deInit},
        {"pauseVideo",         "()V",                                             (void *) videoPause},
        {"resumeVideo",        "()V",                                             (void *) videoResume},
        {"setVideoProgress",   "(J)V",                                            (void *) setProgress}
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

