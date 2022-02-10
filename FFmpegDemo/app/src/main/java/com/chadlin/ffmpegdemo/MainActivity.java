package com.chadlin.ffmpegdemo;

import android.graphics.SurfaceTexture;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.chadlin.ffmpeglib.FFmpegVideoManager;

import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, TextureView.SurfaceTextureListener {
    private TextureView textureView;
    private LocalVideoDataSource localVideoDataSource;
    private SurfaceTexture surface;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.tv_test).setOnClickListener(this);
        findViewById(R.id.tv_play).setOnClickListener(this);
        textureView = findViewById(R.id.surface);
        textureView.setSurfaceTextureListener(this);
        localVideoDataSource = new LocalVideoDataSource(this);
    }

    @RequiresApi(api = Build.VERSION_CODES.Q)
    private List<VideoItem> readVideoFromLocal() {
        return localVideoDataSource.queryData();
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.tv_test:
                Toast.makeText(this, FFmpegVideoManager.getInstance().testConnection(), Toast.LENGTH_SHORT).show();
                break;
            case R.id.tv_play:
                List<VideoItem> list = readVideoFromLocal();
                VideoItem item = list.get(0);
                FFmpegVideoManager.getInstance().playLocalVideo(item.path, new Surface(surface));
                break;
        }
    }

    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
        this.surface = surface;
    }

    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surface) {
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surface) {

    }
}