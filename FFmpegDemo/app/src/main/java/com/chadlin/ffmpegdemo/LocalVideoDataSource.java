package com.chadlin.ffmpegdemo;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.media.MediaMetadataRetriever;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.text.TextUtils;
import android.util.Size;
import android.webkit.MimeTypeMap;

import androidx.annotation.RequiresApi;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.LinkOption;
import java.util.ArrayList;
import java.util.List;

public class LocalVideoDataSource {
    private Context context;

    public LocalVideoDataSource(Context context) {
        this.context = context;
    }

    List<VideoItem> queryData(Uri collection, Uri baseContentUri) {
        List<VideoItem> list = new ArrayList<>();
        String[] projection = new String[]{
                MediaStore.MediaColumns._ID,
                MediaStore.MediaColumns.DISPLAY_NAME,
                MediaStore.MediaColumns.DATA,
                MediaStore.MediaColumns.DURATION,
                MediaStore.MediaColumns.SIZE,
                MediaStore.MediaColumns.DATE_MODIFIED
        };

        try (Cursor cursor = context.getContentResolver().query(
                collection,
                projection,
                null, null, null
        )) {
            // Cache column indices.
            int idColumn = cursor.getColumnIndexOrThrow(MediaStore.MediaColumns._ID);
            int nameColumn =
                    cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.DISPLAY_NAME);
            int pathColumn = cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.DATA);
            int durationColumn =
                    cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.DURATION);
            int sizeColumn = cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.SIZE);
            int dateColumn = cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.DATE_MODIFIED);

            while (cursor.moveToNext()) {
                // Get values of columns for a given video.
                long id = cursor.getLong(idColumn);
                String name = cursor.getString(nameColumn);
                String path = cursor.getString(pathColumn);
                int duration = cursor.getInt(durationColumn);
                int size = cursor.getInt(sizeColumn);
                int date = cursor.getInt(dateColumn);

                Uri contentUri = ContentUris.withAppendedId(
                        baseContentUri, id);
                Bitmap thumbnail = null;
                try {
                    thumbnail = context.getContentResolver().loadThumbnail(contentUri, new Size(300, 300), null);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                // Stores column values and the contentUri in a local object
                // that represents the media file.
                list.add(new VideoItem(contentUri, path, name, thumbnail, duration, size, date));
            }
        }
        return list;
    }

    @RequiresApi(api = Build.VERSION_CODES.Q)
    List<VideoItem> queryData() {
        Uri collection;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            collection = MediaStore.Video.Media.getContentUri(MediaStore.VOLUME_EXTERNAL);
        } else {
            collection = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
        }
        List<VideoItem> list = new ArrayList<>(queryData(collection, MediaStore.Video.Media.EXTERNAL_CONTENT_URI));

        //Query Download
        File downloadFiles = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
        if (downloadFiles.exists() && downloadFiles.canRead()) {
            File[] listFiles = downloadFiles.listFiles();
            if (listFiles != null && listFiles.length > 0) {
                for (File file : listFiles) {
                    String path = file.getAbsolutePath();
                    if (!FileUtils.isVideo(path)) {
                        continue;
                    }
                    MediaMetadataRetriever retriever = new MediaMetadataRetriever();
                    retriever.setDataSource(path);
                    if ("yes".equals(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_HAS_VIDEO))) {
                        String name = file.getName();
                        Bitmap thumbnail = ThumbnailUtils.createVideoThumbnail(path, MediaStore.Video.Thumbnails.MINI_KIND);
                        long duration = Long.parseLong(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION));
                        long size = FileUtils.getFileSize(file);
                        long date = file.lastModified();
                        VideoItem item = new VideoItem(Uri.parse(path),
                                path, name, thumbnail, duration,size, date);
                        list.add(item);
                    }
                    retriever.close();
                }
            }
        }
        return list;
    }
}
