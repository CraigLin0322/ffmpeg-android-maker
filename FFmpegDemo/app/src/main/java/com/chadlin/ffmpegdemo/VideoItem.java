package com.chadlin.ffmpegdemo;

import android.graphics.Bitmap;
import android.net.Uri;

public class VideoItem {
    private final Uri uri;
    private final String name;
    private final int duration;
    private final int size;
    private final Bitmap thumbnail;
    private final int date;
    private final String path;

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
