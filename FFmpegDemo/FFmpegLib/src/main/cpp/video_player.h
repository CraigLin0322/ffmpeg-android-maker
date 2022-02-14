#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

#include "video_player_listener.h"
#include "ffmpeg_define.h"

int play(JNIEnv *env, VideoPlayListener *listener, jstring path, jobject surface, int threadType);


#endif // __VIDEO_PLAYER_H__
