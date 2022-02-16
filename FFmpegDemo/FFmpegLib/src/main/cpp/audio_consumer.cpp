#include "audio_consumer.h"

int AudioConsumer::decodeStream(JNIEnv *env, jobject surface, AVFormatContext *format_context,
                                int stream_index) const {
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
        LOGE(TAG, ": Can not find video stream");
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

void AudioConsumer::resume(JNIEnv *env) const {

}

void AudioConsumer::pause(JNIEnv *env) const {

}

void AudioConsumer::initResource() const {

}

void AudioConsumer::releaseResource() const {

}

int AudioConsumer::play(JNIEnv *env, VideoPlayListener *listener, jstring javaPath,
                        jobject surface) const {

}

void AudioConsumer::seekTo(JNIEnv *env, jlong position) const {

}
