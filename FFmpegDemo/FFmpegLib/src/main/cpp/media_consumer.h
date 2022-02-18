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

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <libswresample/swresample.h>

#ifdef __cplusplus
}
#endif



#endif // __MEDIA_CONSUMER_H__
