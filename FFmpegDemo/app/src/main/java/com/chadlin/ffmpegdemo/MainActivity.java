package com.chadlin.ffmpegdemo;

import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.chadlin.ffmpeglib.FFmpegVideoManager;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, TextureView.SurfaceTextureListener {
    private TextureView textureView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.e("dsads", FFmpegVideoManager.getInstance().testConnection());
        findViewById(R.id.tv_test).setOnClickListener(this);
        textureView = findViewById(R.id.surface);
        textureView.setSurfaceTextureListener(this);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.tv_test:
                Toast.makeText(this, FFmpegVideoManager.getInstance().testConnection(), Toast.LENGTH_SHORT).show();
                break;
        }
    }

    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {

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