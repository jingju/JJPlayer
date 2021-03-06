package com.jingju.jjplayer;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.WindowManager;
import android.widget.MediaController;

import androidx.appcompat.app.AppCompatActivity;

import com.jingju.jjplayer.permission.ActivityPermissionDispatcher;
import com.jingju.jjplayer.permission.PermissionCallback;
import com.jingju.jjplayer.utils.Constant;
import com.jingju.jjplayer.view.JJVideoView;

public class VideoActivity extends AppCompatActivity implements PermissionCallback {

    private MediaController  mMediaController;//todo 媒体控制器
    private JJVideoView mVideoView;
    private String uri= Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/playertest.mp4";
    private String mOutYuvUri = Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/playertest.yuv";
    private String mOutPcmUri= Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/playertest.pcm";

    private String mOutFilePath= Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/out.webm";

    private ActivityPermissionDispatcher dispatcher;
    //    = th()+ "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/bipbop_4x3_variant.m3u8";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) { // 5.0+ 打开硬件加速
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED);
        }
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_video);
        dispatcher = ActivityPermissionDispatcher.getInstance();
        dispatcher.setPermissionCallback(this,this);
        dispatcher.checkedWithStorage(this);
        /**
         * VideoView
         *      player 和 controler相关
         */

        // TODO: 2020/5/21 封装一个播放器的view

        mVideoView = findViewById(R.id.video_view);
        // TODO: 2020/5/11 获取到video path
        mVideoView.setDestFilePath(mOutYuvUri,mOutPcmUri,mOutFilePath);
        mVideoView.setVideoPath(uri);








    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (dispatcher != null) {
            dispatcher.onRequestPermissionResult(this, requestCode, grantResults);
        }

    }
    @Override
    public void onPermission(int request, int state) {
        switch (request){
            case Constant.PERMISSION_READ_STORAGE_REQUEST_CODE:
                switch (state){
                    case SUCCESS:

                        break;
                    case DENIED:
                    case NEVER_ASK_AGAIN:
                        break;
                    case EXPLAIN:
                        break;
                }

                break;
            default:
                break;
        }
    }
}
