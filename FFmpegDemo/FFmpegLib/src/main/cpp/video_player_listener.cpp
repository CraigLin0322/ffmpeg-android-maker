

#include "video_player_listener.h"
#include "ffmpeg_define.h"
#define TAG "video_listener"

VideoPlayListener::VideoPlayListener(JavaVM *vm, JNIEnv *env, jobject obj) {
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
    if (!jmethodProgressId) {
        LOGE(TAG, "Error on getting jmethodStartId");
    }
}

VideoPlayListener::~VideoPlayListener() {
    //TODO
}

void VideoPlayListener::onError(int type, int code, const char *msg) {
    if (type == THREAD_TYPE_SYNC) {
        jstring jmsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, jmethodErrorId, code, jmsg);
        env->DeleteLocalRef(jmsg);
    } else if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmethodErrorId, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);

        vm->DetachCurrentThread();
    }
}

void VideoPlayListener::onProgress(int type,int total, int progress) {
    if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        env->CallVoidMethod(jobj, jmethodProgressId, total, progress);
        vm->DetachCurrentThread();
    } else if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodProgressId, total, progress);
    }
}

void VideoPlayListener::onStart(int type) {
    if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        env->CallVoidMethod(jobj, jmethodStartId);
        vm->DetachCurrentThread();
    } else if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodStartId);
    }
}

void VideoPlayListener::onStop(int type) {
    if (type == THREAD_TYPE_ASYNC) {
        JNIEnv *jniEnv;
        vm->AttachCurrentThread(&jniEnv, 0);
        env->CallVoidMethod(jobj, jmethodEndId);
        vm->DetachCurrentThread();
    } else if (type == THREAD_TYPE_SYNC) {
        env->CallVoidMethod(jobj, jmethodEndId);
    }
}