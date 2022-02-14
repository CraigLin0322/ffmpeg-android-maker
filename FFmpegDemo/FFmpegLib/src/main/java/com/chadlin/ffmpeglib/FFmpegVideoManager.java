package com.chadlin.ffmpeglib;

import android.view.Surface;

public class FFmpegVideoManager {

    static {
        System.loadLibrary("ffmpeglib");
    }

    private static FFmpegVideoManager sInstance;
    private volatile boolean mIsInit;

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

    public int initialize(boolean runInThread) {
        if (mIsInit) {
            return Constants.INITIALIZE_SUCCEED;
        }
        int res = initializeResource(runInThread);
        if (res == Constants.INITIALIZE_SUCCEED) {
            mIsInit = true;
        }
        return res;
    }

    public void release(){
        if (mIsInit) {
            releaseResource();
        }
        mIsInit = false;
    }

    public boolean playVideo(String path, Surface surface, VideoPlayerCallback callback) {
        if (!mIsInit) {
            throw new IllegalStateException("You must call init at first!");
        }
        return playLocalVideo(path, surface, callback);
    }


   private native int initializeResource(boolean runInThread);

    private native void releaseResource();
    /**
     * A native method that is implemented by the 'ffmpeglib' native library,
     * which is packaged with this application.
     */
    private native String testConnection();

    private native boolean playLocalVideo(String localPath, Surface surface, VideoPlayerCallback callback);

    private native void pauseVideo();

    private native void resumeVideo();

    private native void setVideoProgress(long position);
}
