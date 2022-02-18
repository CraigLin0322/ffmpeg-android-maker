package com.chadlin.ffmpegdemo;

import android.text.TextUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URLConnection;

public class FileUtils {
    private static final String PREFIX_VIDEO = "video/";
    public static long getFileSize(File file) {
        long size = 0;
        FileInputStream stream = null;
        if (file.exists() && file.isFile()) {
            try {
                stream = new FileInputStream(file);
                size = stream.available();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                if (stream != null) {
                    try {
                        stream.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return size;
    }

    public static long getFileSize(String path) {
        return getFileSize(new File(path));
    }

    public static boolean isVideo(String path) {
        if (TextUtils.isEmpty(path)) {
            return false;
        }
        String mimeType = getMimeType(path);
        if (TextUtils.isEmpty(mimeType)) {
            return false;
        }
        return mimeType.contains(PREFIX_VIDEO);
    }

    private static String getMimeType(String path) {
        return URLConnection.getFileNameMap().getContentTypeFor(path);
    }
}
