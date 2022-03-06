#ifndef __MEDIA_CONSUMER_H__
#define __MEDIA_CONSUMER_H__

#include <jni.h>
#include "video_player_listener.h"
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <unistd.h>
#include "ffmpeg_define.h"
#include "queue"
#include "vector"
#include <pthread.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

#ifdef __cplusplus
}
#endif

struct MediaContext {
    ANativeWindow *nativeWindow;
    AVFormatContext *formatContext;
    int stream_video_index;
    int stream_audio_index;
};

class MediaConsumer {
public:
    std::vector<AVPacket *> packetQueue;
    pthread_t pthread;

    MediaConsumer() {

    }

    ~MediaConsumer() {

    }

    int put(AVPacket *packet) {
        AVPacket *vaPacket = av_packet_alloc();
        if (av_packet_ref(vaPacket, packet)) {
            //Fail in clone ref
            return 0;
        }
        packetQueue.push_back(vaPacket);
        return 1;
    }

    int get(AVPacket *packet) {
        if (!packetQueue.empty()) {
            if (av_packet_ref(packet, packetQueue.front())) {
                return 0;
            }
            AVPacket *front = packetQueue.front();
            packetQueue.erase(packetQueue.begin());
            av_free(front);
        }
        return 1;
    }


    virtual int play() = 0;

    virtual void seekTo(JNIEnv *env, jlong position) = 0;

    virtual void pause(JNIEnv *env) = 0;

    virtual void resume(JNIEnv *env) = 0;

    virtual void releaseResource() = 0;

    virtual int initResource(MediaContext *context) = 0;

};

#endif // __MEDIA_CONSUMER_H__
