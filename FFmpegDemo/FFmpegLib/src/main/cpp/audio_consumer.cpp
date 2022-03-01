#include "audio_consumer.h"

using namespace AudioConsumer;

AVFormatContext *format_context;
AVCodecContext *av_codec_context;
AVPacket *packet;
AVFrame *frame;
SwrContext *swr_context;
uint8_t *out_buffer;
int out_channel_nb;
int audio_stream_idx = -1;
int rate;
int channel;
static const char *TAG = "AudioConsumer";

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



int getPCM(void **pcm, size_t *pcmSize);

//callback by player
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {

    buffer_size = 0;

    getPCM(&buffer, &buffer_size);
    if (buffer != NULL && buffer_size != 0) {
        //将得到的数据加入到队列中
        (*slBufferQueueItf)->Enqueue(slBufferQueueItf, buffer, buffer_size);
    }
}

//创建引擎
void createEngine() {
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);//创建引擎
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);//实现engineObject接口对象
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
                                  &engineEngine);//通过引擎调用接口初始化SLEngineItf
}

//创建混音器
void createMixVolume() {
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);//用引擎对象创建混音器接口对象
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);//实现混音器接口对象
    SLresult sLresult = (*outputMixObject)->GetInterface(outputMixObject,
                                                         SL_IID_ENVIRONMENTALREVERB,
                                                         &outputMixEnvironmentalReverb);//利用混音器实例对象接口初始化具体的混音器对象
    //设置
    if (SL_RESULT_SUCCESS == sLresult) {
        (*outputMixEnvironmentalReverb)->
                SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &settings);
    }
}

void createBufferQueueAudioPlayer(int bitsPerSample) {
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
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, static_cast<SLuint32>(channel),
                            static_cast<SLuint32>(rate * 1000), SL_PCMSAMPLEFORMAT_FIXED_16,
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


    SLDataLocator_OutputMix slDataLocator_outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};


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
    (*engineEngine)->CreateAudioPlayer(engineEngine, &audioplayer, &dataSource, &slDataSink, 3, ids,
                                       req);
    (*audioplayer)->Realize(audioplayer, SL_BOOLEAN_FALSE);
//    LOGE("执行到此处2");
    (*audioplayer)->GetInterface(audioplayer, SL_IID_PLAY, &slPlayItf);//初始化播放器
    //注册缓冲区,通过缓冲区里面 的数据进行播放
    (*audioplayer)->GetInterface(audioplayer, SL_IID_BUFFERQUEUE, &slBufferQueueItf);
    //设置回调接口
    (*slBufferQueueItf)->RegisterCallback(slBufferQueueItf, bqPlayerCallback, NULL);
    //播放
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);

    //开始播放
    bqPlayerCallback(slBufferQueueItf, NULL);
//    return result;
}

int getPCM(void **pcm, size_t *pcm_size) {
    int frameCount = 0;
    int got_frame;
    while (av_read_frame(format_context, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
//            解码  mp3   编码格式frame----pcm   frame
            avcodec_decode_audio4(av_codec_context, frame, &got_frame, packet);
            if (got_frame) {
//                LOGE("解码");
                /**
                 * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);
                 */
                swr_convert(swr_context, &out_buffer, 44100 * 2, (const uint8_t **) frame->data,
                            frame->nb_samples);
//                缓冲区的大小
                int size = av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
                *pcm = out_buffer;
                *pcm_size = size;
                break;
            }
        }
    }
    return 0;
}

//https://github.com/xufuji456/FFmpegAndroid/blob/master/app/src/main/cpp/opensl_audio_player.cpp
int AudioConsumer::initResource(AVFormatContext *formatContext, int index) {

    createEngine();
    createMixVolume();

    int result;
    audio_stream_idx = index;
    format_context = formatContext;
    av_codec_context = format_context->streams[index]->codec;
    AVCodec *pCodex = avcodec_find_decoder(av_codec_context->codec_id);
    if (pCodex == NULL) {
        return VIDEO_ERROR_FIND_DECODER;
    }
    result = avcodec_open2(av_codec_context, pCodex, NULL);
    if (result < 0) {
        LOGE(TAG, ": Can not find audio stream");
        return VIDEO_ERROR_FIND_VIDEO_STREAM_INFO;
    }

    frame = av_frame_alloc();
    packet = av_packet_alloc();
    swr_context = swr_alloc();
    //    44100*2
    out_buffer = (uint8_t *) av_malloc(44100 * 2);
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
//    输出采样位数  16位
    enum AVSampleFormat out_formart = AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = av_codec_context->sample_rate;


    swr_alloc_set_opts(swr_context, out_ch_layout, out_formart, out_sample_rate,
                       av_codec_context->channel_layout, av_codec_context->sample_fmt, av_codec_context->sample_rate, 0,
                       NULL);

    swr_init(swr_context);
//    获取通道数  2
    out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    rate = av_codec_context->sample_rate;
    channel = av_codec_context->channels;
    createBufferQueueAudioPlayer(SL_PCMSAMPLEFORMAT_FIXED_16);

    return VIDEO_STATUS_SUCCESS

}


void AudioConsumer::releaseResource() {
    swr_free(&swr_context);
    av_free(out_buffer);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avformat_close_input(&format_context);
}

int AudioConsumer::play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath,
                        jobject surface) {

}

void AudioConsumer::seekTo(JNIEnv *env, jlong position) {

}


int AudioConsumer::decodeStream() {
//    bqPlayerCallback(bqPlayerBufferQueue, nullptr);
    return VIDEO_STATUS_SUCCESS
}

void AudioConsumer::resume(JNIEnv *env) {

}

void AudioConsumer::pause(JNIEnv *env) {

}
