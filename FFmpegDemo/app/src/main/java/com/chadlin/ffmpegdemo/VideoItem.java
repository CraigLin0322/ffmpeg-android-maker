package com.chadlin.ffmpegdemo;

import android.graphics.Bitmap;
import android.net.Uri;

public class VideoItem {
     final Uri uri;
     final String name;
     final int duration;
     final int size;
     final Bitmap thumbnail;
     final int date;
     final String path;

    public VideoItem(Uri uri, String path, String name, Bitmap thumbnail, int duration, int size, int date) {
        this.path = path;
        this.date = date;
        this.uri = uri;
        this.name = name;
        this.thumbnail = thumbnail;
        this.duration = duration;
        this.size = size;
    }
}
