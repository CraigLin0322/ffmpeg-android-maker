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


const char *AUDIO_TAG = "VideoConsumer";
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
SLAndroidSimpleBufferQueueItf bpPlayerBufferQueue;
SLEffectSendItf bpPlayerEffectSend;
SLVolumeItf bqPlayerVolume;

//Audio effect

const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
void *openBuffer;
size_t bufferSize;
uint8_t *outputBuffer;
size_t outputBufferSize;

int AudioConsumer::decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context,
                                int stream_index) {
    //TODO change error flag for Audio
    AVCodecContext *audio_codec_context = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(audio_codec_context,
                                  format_context->streams[stream_index]->codecpar);

    AVCodec *audio_codec = avcodec_find_decoder(audio_codec_context->codec_id);
    if (audio_codec == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }
    int result = -1;
    // Open video decoder
    result = avcodec_open2(audio_codec_context, audio_codec, NULL);
    if (result < 0) {
        LOGE(AUDIO_TAG, ": Can not find video stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }
    //申请avpakcet，装解码前的数据
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    //申请avframe，装解码后的数据
    AVFrame *frame = av_frame_alloc();

    //得到SwrContext ，进行重采样，具体参考http://blog.csdn.net/jammg/article/details/52688506
    SwrContext *swrContext = swr_alloc();
    //缓存区
    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
    //输出的声道布局（立体声）
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    //Output sampleSize, basically for 16
    enum AVSampleFormat sampleFormat = AV_SAMPLE_FMT_S16;
    //输出的采样率必须与输入相同
    int out_sample_rate = audio_codec_context->sample_rate;

    //swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
    swr_alloc_set_opts(swrContext, out_ch_layout, sampleFormat, out_sample_rate,
                       audio_codec_context->channel_layout, audio_codec_context->sample_fmt,
                       audio_codec_context->sample_rate, 0,
                       NULL);
    swr_init(swrContext);
    //    获取通道数  2
    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    int got_frame;
    while (av_read_frame(format_context, packet) >= 0) {
        if (packet->stream_index == stream_index) {
//           decode mp3
            avcodec_decode_audio4(audio_codec_context, frame, &got_frame, packet);
            if (got_frame) {
                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data,
                            frame->nb_samples);
                //setup buffer
                int size = av_samples_get_buffer_size(NULL, out_channer_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
//                    *pcm = out_buffer;
//                    *pcm_size = size;
            }
        }
    }
    swr_free(&swrContext);
    av_free(out_buffer);
    av_frame_free(&frame);
    av_packet_free(&packet);
    return VIDEO_STATUS_SUCCESS
}

void AudioConsumer::resume(JNIEnv *env) {

}

void AudioConsumer::pause(JNIEnv *env) {

}

void AudioConsumer::bpPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    int bufferSize = 0;
//    getPcm(&openBuffer, &bufferSize);
    if (nullptr != &openBuffer && 0 != &bufferSize) {
        audioResult = (*bpPlayerBufferQueue)->Enqueue(bpPlayerBufferQueue, openBuffer, bufferSize);
        if (audioResult < 0) {

        } else {
            //frame_count++
        }
    }
}

void initBufferQueue(int rate, int channel, int bitsPerSample) {
    //Init audio buffer queue

    SLDataLocator_AndroidSimpleBufferQueue bufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                          2};
    SLDataFormat_PCM formatPcm;
    formatPcm.formatType = SL_DATAFORMAT_PCM;
    formatPcm.numChannels = (SLuint32) channel;
    formatPcm.bitsPerSample = (SLuint32) bitsPerSample;
    formatPcm.samplesPerSec = (SLuint32) (rate * 1000);
    formatPcm.containerSize = 16;
    if (channel == 2) {
        formatPcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    } else {
        formatPcm.channelMask = SL_SPEAKER_FRONT_CENTER;
    }

    formatPcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource audioSrc = {&bufferQueue, &formatPcm};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSink{&loc_outmix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    audioResult = (*engineEngine)->CreateAudioPlayer(engineEngine, &bpPlayerObject,
                                                     &audioSrc, &audioSink, 3, ids, req);
    if (audioResult != SL_RESULT_SUCCESS) {

    }

    audioResult = (*bpPlayerObject)->Realize(bpPlayerObject, SL_BOOLEAN_FALSE);
    if (audioResult != SL_RESULT_SUCCESS) {

    }
    (*bpPlayerObject)->GetInterface(bpPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    (*bpPlayerObject)->GetInterface(bpPlayerObject, SL_IID_BUFFERQUEUE, &bpPlayerBufferQueue);

    audioResult = (*bpPlayerBufferQueue)->RegisterCallback(bpPlayerBufferQueue,
                                                           bpPlayerCallback,
                                                           nullptr);
    if (audioResult != SL_RESULT_SUCCESS) {

    }
    (*bpPlayerObject)->GetInterface(bpPlayerObject, SL_IID_EFFECTSEND, &bpPlayerEffectSend);
    (*bpPlayerObject)->GetInterface(bpPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    audioResult = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
}

void prepareDecodeContext() {
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    frame = av_frame_alloc();
    swrContext = swr_alloc();
    out_buffer = (uint8_t *) av_malloc(44100 * 2);
    out_sample_rate = audio_codec_context->sample_rate;
    //swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
    swr_alloc_set_opts(swrContext, out_ch_layout, sampleFormat, out_sample_rate,
                       audio_codec_context->channel_layout, audio_codec_context->sample_fmt,
                       audio_codec_context->sample_rate, 0,
                       NULL);
    swr_init(swrContext);
    out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
}

int initAudioEngine() {
    SLresult result;
    result = slCreateEngine(&engineObject, 0, nullptr,
                            0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvReverb);
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*outputMixEnvReverb)->SetEnvironmentalReverbProperties(
            outputMixEnvReverb, &reverbSettings);
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
        LOGE(AUDIO_TAG, ": Can not find audio stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }

    initAudioEngine();

    initBufferQueue(audio_codec_context->sample_rate, audio_codec_context->channels,
                    SL_PCMSAMPLEFORMAT_FIXED_16);

    prepareDecodeContext();

    return VIDEO_STATUS_SUCCESS

}


void AudioConsumer::releaseResource() {

}

int AudioConsumer::play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath,
                        jobject surface) {

}

void AudioConsumer::seekTo(JNIEnv *env, jlong position) {

}
