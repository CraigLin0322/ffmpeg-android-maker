package com.chadlin.ffmpegdemo;

import com.chadlin.ffmpeglib.FFmpegVideoManager;
import com.chadlin.ffmpeglib.VideoPlayerCallback;

import android.graphics.SurfaceTexture;
import android.hardware.display.DisplayManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.Toast;

import java.util.List;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

/**
 * Token for pushing to Git: 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
 *
 * Mac: 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
 */
public class MainActivity extends AppCompatActivity implements View.OnClickListener, TextureView.SurfaceTextureListener {
    private static final String TAG = MainActivity.class.getSimpleName();
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
//                Toast.makeText(this, FFmpegVideoManager.getInstance().testConnection(), Toast.LENGTH_SHORT).show();
                break;
            case R.id.tv_play:
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        List<VideoItem> list = readVideoFromLocal();
                        VideoItem item = list.get(0);
                        FFmpegVideoManager.getInstance().playVideo(item.path, new Surface(surface), new VideoPlayerCallback() {
                            @Override
                            public void onVideoStart() {
                                Log.e(TAG, "onVideoStart");
                            }

                            @Override
                            public void onProgress(int total, int current) {
                                Log.e(TAG, "onProgress, total=" + total + " current=" + current);
                            }

                            @Override
                            public void onVideoStop() {
                                Log.e(TAG, "onVideoStop");
                            }

                            @Override
                            public void onError(int code, String msg) {
                                Log.e(TAG, "onError=" + msg);
                            }
                        });

                    }
                }).start();
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