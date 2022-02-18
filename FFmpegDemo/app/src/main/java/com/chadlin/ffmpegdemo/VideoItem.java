package com.chadlin.ffmpegdemo;

import android.graphics.Bitmap;
import android.net.Uri;

public class VideoItem {
     final Uri uri;
     final String name;
     final long duration;
     final long size;
     final Bitmap thumbnail;
     final long date;
     final String path;

    public VideoItem(Uri uri, String path, String name, Bitmap thumbnail, long duration, long size, long date) {
        this.path = path;
        this.date = date;
        this.uri = uri;
        this.name = name;
        this.thumbnail = thumbnail;
        this.duration = duration;
        this.size = size;
    }
}
