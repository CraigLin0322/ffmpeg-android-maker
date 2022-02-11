#ifndef __VIDEO_PLAYER_H__
#define __VIDEO_PLAYER_H__

#include "video_player_listener.h"

#define ERROR_OPEN 0
#define ERROR_FIND_VIDEO_STREAM_INFO 1
#define ERROR_FIND_VIDEO_STREAM 2
#define ERROR_FIND_DECODER  3
#define ERROR_OPEN_DECODER  4
#define ERROR_CREATE_NATIVE_WINDOW  5
#define ERROR_SET_NATIVE_WINDOW_BUFFER 6
#define ERROR_DECODE_FAIL 7
#define ERROR_RECEIVE_FRAME 8
#define ERROR_CONVERT_DATA 9
#define ERROR_LOCK_NATIVE_WINDOW 10





int play(JNIEnv *env, VideoPlayListener *listener, jstring path, jobject surface, int threadType);


#endif // __VIDEO_PLAYER_H__
