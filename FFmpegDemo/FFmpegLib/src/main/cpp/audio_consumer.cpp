#include "audio_consumer.h"

using namespace AudioConsumer;

AVCodecContext *audio_codec_context;
AVFormatContext *avFormatContext;
int audio_stream_index = -1;
AVCodec *audio_codec;


//Encapsulate for raw data
AVPacket *packet;
//Encapsulate for decoded data
AVFrame *frame;
SwrContext *swrContext;
//Output buffer
uint8_t *out_buffer;
//Channel_layout
const uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
//Output sampleSize, basically for 16
const enum AVSampleFormat sampleFormat = AV_SAMPLE_FMT_S16;
int out_sample_rate;
int out_channel_nb;
int frame_count = 0;

static const char *TAG = "VideoConsumer";
SLresult audioResult;
//Object of engine
SLObjectItf engineObject = nullptr;
SLEngineItf engineEngine;

//Object of mixer
SLObjectItf outputMixObject = nullptr;
SLEnvironmentalReverbItf outputMixEnvReverb = nullptr;

//Object of buffer
SLObjectItf bqPlayerObject = nullptr;
SLPlayItf bqPlayerPlay;
SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
SLEffectSendItf bpPlayerEffectSend;
SLVolumeItf bqPlayerVolume;

//Audio effect

const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

void *openBuffer;
size_t bufferSize;
size_t outputBufferSize;


int getPcm(void **pcm, size_t *pcmSize) {
    int frameFinish = 0;
    int data_size = 0;
    while (av_read_frame(avFormatContext, packet) >= 0) {
        if (packet->stream_index == audio_stream_index) {
            avcodec_decode_audio4(audio_codec_context, frame, &frameFinish, packet);
            if (frameFinish) {
                data_size = av_samples_get_buffer_size(frame->linesize, audio_codec_context->channels,
                                                       frame->nb_samples,
                                                       audio_codec_context->sample_fmt, 1);
                if (data_size > outputBufferSize) {
                    outputBufferSize = data_size;
                    out_buffer = (uint8_t *) realloc(out_buffer,
                                                     sizeof(uint8_t) * outputBufferSize);
                }
                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data,
                            frame->nb_samples);
                *pcm = out_buffer;
                *pcmSize = data_size;
                break;
            }
        }
    }
    return 0;
}

void bpPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    getPcm(&openBuffer, &bufferSize);
    if (nullptr != &openBuffer && 0 != &bufferSize) {
        audioResult = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, openBuffer, bufferSize);
        if (audioResult < 0) {

        } else {
            frame_count++;
        }
    }
}

void initBufferQueue(int rate, int channel, int bitsPerSample) {
    //Init audio buffer queue

    SLresult result;

    //config audio source
    SLDataLocator_AndroidSimpleBufferQueue buffer_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                           2};
    SLDataFormat_PCM format_pcm;
    format_pcm.formatType = SL_DATAFORMAT_PCM;
    format_pcm.numChannels = (SLuint32) channel;
    format_pcm.bitsPerSample = (SLuint32) bitsPerSample;
    format_pcm.samplesPerSec = (SLuint32) (rate * 1000);
    format_pcm.containerSize = 16;
    if (channel == 2)
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    else
        format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource audioSrc = {&buffer_queue, &format_pcm};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "outputMixObject->GetInterface error=%d", result);
        return;
    }
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "bqPlayerObject->Realize error=%d", result);
        return ;
    }
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bpPlayerCallback, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "bqPlayerBufferQueue->RegisterCallback error=%d", result);
        return ;
    }
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND, &bpPlayerEffectSend);
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
}

void prepareDecodeContext() {
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    frame = av_frame_alloc();
    swrContext = swr_alloc();
    out_buffer = (uint8_t *) av_malloc(44100 * 2);
    out_sample_rate = audio_codec_context->sample_rate;
    //swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
    swr_alloc_set_opts(swrContext,  audio_codec_context->channel_layout, sampleFormat, audio_codec_context->sample_rate,
                       audio_codec_context->channel_layout, audio_codec_context->sample_fmt,
                       audio_codec_context->sample_rate, 0,
                       NULL);
//    aFrame = av_frame_alloc();
//    swr = swr_alloc();
//    av_opt_set_int(swr, "in_channel_layout", aCodecCtx->channel_layout, 0);
//    av_opt_set_int(swr, "out_channel_layout", aCodecCtx->channel_layout, 0);
//    av_opt_set_int(swr, "in_sample_rate", aCodecCtx->sample_rate, 0);
//    av_opt_set_int(swr, "out_sample_rate", aCodecCtx->sample_rate, 0);
//    av_opt_set_sample_fmt(swr, "in_sample_fmt", aCodecCtx->sample_fmt, 0);
//    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
//    swr_init(swr);

    swr_init(swrContext);
    out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
}

int initAudioEngine() {
    SLresult result;
    result = slCreateEngine(&engineObject, 0, nullptr,
                            0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "slCreateEngine error=%d", result);
        return result;
    }
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "engineObject->Realize error=%d", result);
        return result;
    }
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "engineObject->GetInterface error=%d", result);
        return result;
    }
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "engineEngine->CreateOutputMix error=%d", result);
        return result;
    }
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(TAG, "outputMixObject->Realize error=%d", result);
        return result;
    }
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvReverb);
    if (result == SL_RESULT_SUCCESS) {
        result = (*outputMixEnvReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvReverb, &reverbSettings);
    } else {
        LOGE(TAG, "outputMixObject->GetInterface error=%d", result);
        return result;
    }

    return result;
}

//https://github.com/xufuji456/FFmpegAndroid/blob/master/app/src/main/cpp/opensl_audio_player.cpp
int AudioConsumer::initResource(AVFormatContext *formatContext, int index) {

    int result;
    audio_stream_index = index;
    avFormatContext = formatContext;
    audio_codec_context = avFormatContext->streams[audio_stream_index]->codec;
    audio_codec = avcodec_find_decoder(audio_codec_context->codec_id);
    if (audio_codec == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }
    result = avcodec_open2(audio_codec_context, audio_codec, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not find audio stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }

    initAudioEngine();

    initBufferQueue(audio_codec_context->sample_rate, audio_codec_context->channels,
                    SL_PCMSAMPLEFORMAT_FIXED_16);

    prepareDecodeContext();

    return VIDEO_STATUS_SUCCESS

}


void AudioConsumer::releaseResource() {
    swr_free(&swrContext);
    av_free(out_buffer);
    av_frame_free(&frame);
    av_packet_free(&packet);
    frame_count = 0;
}

int AudioConsumer::play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath,
                        jobject surface) {

}

void AudioConsumer::seekTo(JNIEnv *env, jlong position) {

}


int AudioConsumer::decodeStream() {
    bpPlayerCallback(bqPlayerBufferQueue, nullptr);
    return VIDEO_STATUS_SUCCESS
}

void AudioConsumer::resume(JNIEnv *env) {

}

void AudioConsumer::pause(JNIEnv *env) {

}
