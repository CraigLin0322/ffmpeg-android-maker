package com.chadlin.ffmpeglib;

import android.view.Surface;

public class FFmpegVideoManager {

    static {
        System.loadLibrary("ffmpeglib");
    }

    private static FFmpegVideoManager sInstance;

    private FFmpegVideoManager() {

    }

    public static FFmpegVideoManager getInstance(){
        if (sInstance == null) {
            synchronized (FFmpegVideoManager.class) {
                if (sInstance == null) {
                    sInstance = new FFmpegVideoManager();
                }
            }
        }
        return sInstance;
    }


    /**
     * A native method that is implemented by the 'ffmpeglib' native library,
     * which is packaged with this application.
     */
    public native String testConnection();

    public native boolean playLocalVideo(String localPath, Surface surface, VideoPlayerCallback callback);
}
