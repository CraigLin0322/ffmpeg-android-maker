

#include "video_player_listener.h"
#include "ffmpeg_define.h"

#define TAG "video_listener"

VideoPlayListener::VideoPlayListener(JavaVM *vm, JNIEnv *env, jobject obj, jboolean runOnThread) {
    this->vm = vm;
    this->env = env;
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
    type = runOnThread ? THREAD_TYPE_ASYNC : THREAD_TYPE_SYNC;
}

VideoPlayListener::~VideoPlayListener() {
    //TODO
}

void VideoPlayListener::onError(int code) const {
    if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodErrorId, code);
    } else if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        jniEnv->CallVoidMethod(jobj, jmethodErrorId, code);
        vm->DetachCurrentThread();
    }
}

void VideoPlayListener::onProgress(int total, int progress) const {
    if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        env->CallVoidMethod(jobj, jmethodProgressId, total, progress);
        vm->DetachCurrentThread();
    } else if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodProgressId, total, progress);
    }
}

void VideoPlayListener::onStart() const {
    if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        env->CallVoidMethod(jobj, jmethodStartId);
        vm->DetachCurrentThread();
    } else if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodStartId);
    }
}

void VideoPlayListener::onStop() const {
    if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        env->CallVoidMethod(jobj, jmethodEndId);
        vm->DetachCurrentThread();
    } else if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodEndId);
    }
}