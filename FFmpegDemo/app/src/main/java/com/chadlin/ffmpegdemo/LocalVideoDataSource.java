package com.chadlin.ffmpegdemo;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.util.Size;

import androidx.annotation.RequiresApi;

import java.io.IOException;
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
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            collection = MediaStore.Downloads.getContentUri(MediaStore.VOLUME_EXTERNAL);
        } else {
            collection = MediaStore.Downloads.EXTERNAL_CONTENT_URI;
        }
        list.addAll(queryData(collection, MediaStore.Downloads.EXTERNAL_CONTENT_URI));
        return list;
    }
}
