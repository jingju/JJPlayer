package com.jingju.jjplayer;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.MediaController;

import androidx.appcompat.app.AppCompatActivity;

public class VideoActivity extends AppCompatActivity {

    private String mVideoPath;
    private MediaController  mMediaController;//todo 媒体控制器
    private SurfaceView mShowView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        /**
         * VideoView
         *      player 和 controler相关
         */
        mShowView = findViewById(R.id.sf_show);
        // TODO: 2020/5/11 获取到video path

        videoview (player )



        mShowView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Surface surface = holder.getSurface();
                // TODO: 2020/5/16 设置给jni层，创建window相关数据


            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {


            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });




    }
}
