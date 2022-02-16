#ifndef __AUDIO_CONSUMER_H__
#define __AUDIO_CONSUMER_H__

#include "media_consumer.h"
#include "jni.h"
#include "cstring"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

class AudioConsumer : public virtual MediaConsumer {
protected:
    const char *TAG = "VideoConsumer";
    SLresult audioResult;
    //Object of engine
    SLObjectItf engineObject = nullptr;
    SLEngineItf engineEngine;

    //Object of mixer
    SLObjectItf outputMixObject = nullptr;
    SLEnvironmentalReverbItf outputMixEnvReverb = nullptr;

    //Object of buffer
    SLObjectItf bpPlayerObject = nullptr;
    SLPlayItf bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf_ bpPlayerBufferQueue;
    SLEffectSendItf  bpPlayerEffectSend;
    SLVolumeItf  bqPlayerVolume;

    //Audio effect

    const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    void * openBuffer;
    size_t bufferSize;
    uint8_t * outputBuffer;
    size_t outputBufferSize;

public:

    int decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context,
                     int stream_index) const override;

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface) const override;

    void seekTo(JNIEnv *env, jlong position) const override;

    void pause(JNIEnv *env) const override;

    void resume(JNIEnv *env) const override;

    void releaseResource() override;

    void initResource() override;

    ~AudioConsumer() {

    }
};


#endif // __AUDIO_CONSUMER_H__
