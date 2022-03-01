

#include "video_player_listener.h"
#include "ffmpeg_define.h"

#define TAG "video_listener"

bool isEnvAttach(JavaVM *vm, JNIEnv *jniEnv) {
    return vm->GetEnv((void **) &jniEnv, JNI_VERSION_1_4) == JNI_OK;
}

VideoPlayListener::VideoPlayListener(JavaVM *jvm, JNIEnv *jenv, jobject obj, jboolean runOnThread) {
    this->vm = jvm;
    this->env = jenv;
    this->jobj = obj;
    jclass jclazz = env->GetObjectClass(jobj);
    if (!jclazz) {
        LOGE(TAG, "Error on getting jclass");
        return;
    }
    jmethodErrorId = env->GetMethodID(jclazz, "onError", "(ILjava/lang/String;)V");
    if (!jmethodErrorId) {
        LOGE(TAG, "Error on getting jmethodErrorId");
    }

    jmethodEndId = env->GetMethodID(jclazz, "onVideoStop", "()V");
    if (!jmethodEndId) {
        LOGE(TAG, "Error on getting jmethodEndId");
    }

    jmethodProgressId = env->GetMethodID(jclazz, "onProgress", "(II)V");
    if (!jmethodProgressId) {
        LOGE(TAG, "Error on getting jmethodProgressId");
    }

    jmethodStartId = env->GetMethodID(jclazz, "onVideoStart", "()V");
    if (!jmethodStartId) {
        LOGE(TAG, "Error on getting jmethodStartId");
    }
}

VideoPlayListener::~VideoPlayListener() {
    //TODO
}

void VideoPlayListener::onError(int code) const {
    JNIEnv *jniEnv;
    if (isEnvAttach(vm, jniEnv)) {
        env->CallVoidMethod(jobj, jmethodErrorId, code);
    } else {
        vm->AttachCurrentThread(&jniEnv, nullptr);
        jniEnv->CallVoidMethod(jobj, jmethodErrorId, code);
        vm->DetachCurrentThread();
    }
}

void VideoPlayListener::onProgress(int total, int progress) const {
    JNIEnv *jniEnv;
    if (isEnvAttach(vm, jniEnv)) {
        env->CallVoidMethod(jobj, jmethodProgressId, total, progress);
    } else {
        vm->AttachCurrentThread(&jniEnv, nullptr);
        jniEnv->CallVoidMethod(jobj, jmethodProgressId, total, progress);
        vm->DetachCurrentThread();
    }
}

void VideoPlayListener::onStart() const {
    JNIEnv *jniEnv;
    if (isEnvAttach(vm, jniEnv)) {
        env->CallVoidMethod(jobj, jmethodStartId);
    } else {
        vm->AttachCurrentThread(&jniEnv, nullptr);
        jniEnv->CallVoidMethod(jobj, jmethodStartId);
        vm->DetachCurrentThread();
    }
}

void VideoPlayListener::onStop() const {
    JNIEnv *jniEnv;
    if (isEnvAttach(vm, jniEnv)) {
        env->CallVoidMethod(jobj, jmethodEndId);
    } else {
        vm->AttachCurrentThread(&jniEnv, nullptr);
        jniEnv->CallVoidMethod(jobj, jmethodEndId);
        vm->DetachCurrentThread();
    }
}