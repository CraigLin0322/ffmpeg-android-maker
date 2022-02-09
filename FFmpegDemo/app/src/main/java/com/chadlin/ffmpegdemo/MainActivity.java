package com.chadlin.ffmpegdemo;

import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.chadlin.ffmpeglib.FFmpegVideoManager;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.e("dsads", FFmpegVideoManager.getInstance().stringFromJNI());

    }
}