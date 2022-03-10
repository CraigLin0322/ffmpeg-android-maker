#include "audio_consumer.h"

static const char *TAG = "AudioConsumer";
static const int BUFFER_SIZE = 44100 * 2;
int getPCM(AudioConsumer *consumer);

//callback by player
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {

    auto *consumer = static_cast<AudioConsumer *>(context);

    int size = getPCM(consumer);
    if (consumer->buffer != NULL && size != 0) {
        double time = size / (BUFFER_SIZE * 2);
        consumer->clock += time;
        (*consumer->slBufferQueueItf)->Enqueue(consumer->slBufferQueueItf, consumer->buffer,
                                               size);
    }
}

void * loopAudioFrames(void * args) {
    auto *consumer = static_cast<AudioConsumer *>(args);
    bqPlayerCallback(consumer->slBufferQueueItf, consumer);
    pthread_exit((void *) 2);
}

void createEngine(AudioConsumer *consumer) {
    slCreateEngine(&consumer->engineObject, 0, NULL, 0, NULL, NULL);//创建引擎
    (*consumer->engineObject)->Realize(consumer->engineObject,
                                       SL_BOOLEAN_FALSE);
    (*consumer->engineObject)->GetInterface(consumer->engineObject, SL_IID_ENGINE,
                                            &consumer->engineEngine);
}

void createMixVolume(AudioConsumer *consumer) {
    (*consumer->engineEngine)->CreateOutputMix(consumer->engineEngine, &consumer->outputMixObject,
                                               0, 0, 0);
    (*consumer->outputMixObject)->Realize(consumer->outputMixObject, SL_BOOLEAN_FALSE);
    SLresult sLresult = (*consumer->outputMixObject)->GetInterface(consumer->outputMixObject,
                                                                   SL_IID_ENVIRONMENTALREVERB,
                                                                   &consumer->outputMixEnvironmentalReverb);//利用混音器实例对象接口初始化具体的混音器对象
    //设置
    if (SL_RESULT_SUCCESS == sLresult) {
        (*consumer->outputMixEnvironmentalReverb)->
                SetEnvironmentalReverbProperties(consumer->outputMixEnvironmentalReverb,
                                                 &consumer->settings);
    }
}

void createBufferQueueAudioPlayer(AudioConsumer *consumer, int bitsPerSample) {
    /*
   * typedef struct SLDataLocator_AndroidBufferQueue_ {
  SLuint32    locatorType;//缓冲区队列类型
  SLuint32    numBuffers;//buffer位数
} */

    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    /**
    typedef struct SLDataFormat_PCM_ {
        SLuint32 		formatType;  pcm
        SLuint32 		numChannels;  通道数
        SLuint32 		samplesPerSec;  采样率
        SLuint32 		bitsPerSample;  采样位数
        SLuint32 		containerSize;  包含位数
        SLuint32 		channelMask;     立体声
        SLuint32		endianness;    end标志位
    } SLDataFormat_PCM;
     */
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, static_cast<SLuint32>(consumer->channel),
                            static_cast<SLuint32>(consumer->rate * 1000),
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                            SL_BYTEORDER_LITTLEENDIAN};

    /*
     * typedef struct SLDataSource_ {
	        void *pLocator;//缓冲区队列
	        void *pFormat;//数据样式,配置信息
        } SLDataSource;
     * */
    SLDataSource dataSource = {&android_queue, &pcm};


    SLDataLocator_OutputMix slDataLocator_outputMix = {SL_DATALOCATOR_OUTPUTMIX,
                                                       consumer->outputMixObject};


    SLDataSink slDataSink = {&slDataLocator_outputMix, NULL};


    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE};

    /*
     * SLresult (*CreateAudioPlayer) (
		SLEngineItf self,
		SLObjectItf * pPlayer,
		SLDataSource *pAudioSrc,//数据设置
		SLDataSink *pAudioSnk,//关联混音器
		SLuint32 numInterfaces,
		const SLInterfaceID * pInterfaceIds,
		const SLboolean * pInterfaceRequired
	);
     * */
//    LOGE("执行到此处")
    (*consumer->engineEngine)->CreateAudioPlayer(consumer->engineEngine, &consumer->audioplayer,
                                                 &dataSource, &slDataSink, 3, ids,
                                                 req);
    (*consumer->audioplayer)->Realize(consumer->audioplayer, SL_BOOLEAN_FALSE);
//    LOGE("执行到此处2");
    (*consumer->audioplayer)->GetInterface(consumer->audioplayer, SL_IID_PLAY,
                                           &consumer->slPlayItf);//初始化播放器
    //注册缓冲区,通过缓冲区里面 的数据进行播放
    (*consumer->audioplayer)->GetInterface(consumer->audioplayer, SL_IID_BUFFERQUEUE,
                                           &consumer->slBufferQueueItf);
    //设置回调接口
    (*consumer->slBufferQueueItf)->RegisterCallback(consumer->slBufferQueueItf, bqPlayerCallback,
                                                    consumer);
    //播放
    (*consumer->slPlayItf)->SetPlayState(consumer->slPlayItf, SL_PLAYSTATE_PLAYING);


//    return result;
}

int getPCM(AudioConsumer *consumer) {
    int frameCount = 0;
    int got_frame;
    void **pcm =&consumer->buffer ;
    int size = 0;
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    while (consumer->get(packet)) {
        if (packet->stream_index == consumer->audio_stream_index) {
            if (packet->pts != AV_NOPTS_VALUE) {
                consumer->clock = av_q2d(consumer->av_codec_context->time_base) * packet->pts;
            }
            avcodec_decode_audio4(consumer->av_codec_context, frame, &got_frame,
                                  packet);
            if (got_frame) {
                /**
                 * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);
                 */
                swr_convert(consumer->swr_context, &consumer->out_buffer, BUFFER_SIZE,
                            (const uint8_t **) frame->data,
                            frame->nb_samples);
                size = av_samples_get_buffer_size(NULL, consumer->out_channel_nb,
                                                      frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
                *pcm = consumer->out_buffer;
                break;
            }
        }
    }
    av_free(packet);
    av_frame_free(&frame);
    return size;
}

//https://github.com/xufuji456/FFmpegAndroid/blob/master/app/src/main/cpp/opensl_audio_player.cpp
int AudioConsumer::initResource(MediaContext* mediaContext) {
    createEngine(this);
    createMixVolume(this);

    int result;
    formatContext = mediaContext->formatContext;
    av_codec_context = formatContext->streams[mediaContext->stream_audio_index]->codec;
    audio_stream_index = mediaContext->stream_audio_index;
    AVCodec *pCodex = avcodec_find_decoder(av_codec_context->codec_id);
    if (pCodex == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }
    result = avcodec_open2(av_codec_context, pCodex, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not find audio stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }

    swr_context = swr_alloc();
    out_buffer = (uint8_t *) av_malloc(BUFFER_SIZE);
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
//    输出采样位数  16位
    enum AVSampleFormat out_formart = AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = av_codec_context->sample_rate;


    swr_alloc_set_opts(swr_context, out_ch_layout, out_formart, out_sample_rate,
                       av_codec_context->channel_layout, av_codec_context->sample_fmt,
                       av_codec_context->sample_rate, 0,
                       NULL);

    swr_init(swr_context);
//    获取通道数  2
    out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    rate = av_codec_context->sample_rate;
    channel = av_codec_context->channels;
    createBufferQueueAudioPlayer(this, SL_PCMSAMPLEFORMAT_FIXED_16);

    return VIDEO_STATUS_SUCCESS

}


void AudioConsumer::releaseResource() {
    swr_free(&swr_context);
    av_free(out_buffer);
    avformat_close_input(&formatContext);
}


void AudioConsumer::seekTo(JNIEnv *env, jlong position) {

}

int AudioConsumer::play() {
    //开始播放
    pthread_create(&pthread, NULL, loopAudioFrames, this);
    return VIDEO_STATUS_SUCCESS
}

void AudioConsumer::resume(JNIEnv *env) {

}

void AudioConsumer::pause(JNIEnv *env) {

}

AudioConsumer::AudioConsumer() {

}

AudioConsumer::~AudioConsumer() {

}
