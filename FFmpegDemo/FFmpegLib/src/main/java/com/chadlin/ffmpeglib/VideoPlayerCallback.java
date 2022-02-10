package com.chadlin.ffmpeglib;


public interface VideoPlayerCallback {

   void onVideoStart();

   void onProgress(int total, int current);

   void onVideoStop();
}
