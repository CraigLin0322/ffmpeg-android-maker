package com.chadlin.ffmpegdemo;

import android.util.Log;

import androidx.activity.ComponentActivity;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.contract.ActivityResultContracts.RequestMultiplePermissions;

import java.lang.ref.WeakReference;
import java.util.Map;

public class PermissionService {
    public void checkPermission(ComponentActivity activity, String... permission ) {
        WeakReference<ComponentActivity> weakReference = new WeakReference<>(activity);
        String[] permissions = permission;
        ComponentActivity activityRef = weakReference.get();
        if (activityRef != null) {
            activity.registerForActivityResult(new RequestMultiplePermissions(), new ActivityResultCallback<Map<String, Boolean>>() {
                @Override
                public void onActivityResult(Map<String, Boolean> result) {
                    Log.e("dsadsa", "dsada");
                    //TODO Fill logic
                }
            }).launch(permissions);
        }
    }


    public interface PermissionCallback {
        void onSuccess(String[] permissions);

        void onFail(String[] permissions);
    }
}
