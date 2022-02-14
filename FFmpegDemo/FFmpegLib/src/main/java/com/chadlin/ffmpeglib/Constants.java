package com.chadlin.ffmpeglib;


public class Constants {

    //Status for initializing
    public static final int INITIALIZE_SUCCEED= 0;
    public static final int INITIALIZE_FAIL =1;

    //Status for playing video
    public static final int VIDEO_STATUS_SUCCESS = 0;
    public static final int VIDEO_STATUS_FAILURE = -1;
    public static final int VIDEO_ERROR_OPEN = 0;
    public static final int VIDEO_ERROR_FIND_VIDEO_STREAM_INFO = 1;
    public static final int VIDEO_ERROR_FIND_VIDEO_STREAM = 2;
    public static final int VIDEO_ERROR_FIND_DECODER = 3;
    public static final int VIDEO_ERROR_OPEN_DECODER = 4;
    public static final int VIDEO_ERROR_CREATE_NATIVE_WINDOW = 5;
    public static final int VIDEO_ERROR_SET_NATIVE_WINDOW_BUFFER = 6;
    public static final int VIDEO_ERROR_DECODE_FAIL = 7;
    public static final int VIDEO_ERROR_RECEIVE_FRAME = 8;
    public static final int VIDEO_ERROR_CONVERT_DATA = 9;
    public static final int VIDEO_ERROR_LOCK_NATIVE_WINDOW = 10;
}
