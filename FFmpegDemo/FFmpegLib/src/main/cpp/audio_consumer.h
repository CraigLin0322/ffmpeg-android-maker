#ifndef __AUDIO_CONSUMER_H__
#define __AUDIO_CONSUMER_H__

#include "media_consumer.h"
#include "jni.h"
#include "cstring"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

class AudioConsumer :MediaConsumer {

public:
    AVFormatContext *formatContext;
    AVCodecContext *av_codec_context;
    AVPacket *packet;
    AVFrame *frame;
    SwrContext *swr_context;
    uint8_t *out_buffer;
    int out_channel_nb;
    int audio_stream_idx = -1;
    int rate;
    int channel;

    size_t buffer_size = 0;
    void *buffer;
    SLObjectItf engineObject = NULL;//用SLObjectItf声明引擎接口对象
    SLEngineItf engineEngine = NULL;//声明具体的引擎对象


    SLObjectItf outputMixObject = NULL;//用SLObjectItf创建混音器接口对象
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;////具体的混音器对象实例
    SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;//默认情况


    SLObjectItf audioplayer = NULL;//用SLObjectItf声明播放器接口对象
    SLPlayItf slPlayItf = NULL;//播放器接口
    SLAndroidSimpleBufferQueueItf slBufferQueueItf = NULL;//缓冲区队列接口


    int decodeStream();

    int play(JNIEnv *env, VideoPlayListener *listener,
             jstring javaPath, jobject surface);

    void seekTo(JNIEnv *env, jlong position);

    void pause(JNIEnv *env);

    void resume(JNIEnv *env);

    void releaseResource();

    int initResource(MediaContext *mediaContext);

    AudioConsumer();

    ~AudioConsumer();

};

#endif // __AUDIO_CONSUMER_H__
