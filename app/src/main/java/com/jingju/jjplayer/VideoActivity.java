package com.jingju.jjplayer;
import android.os.Bundle;
import android.widget.MediaController;

import androidx.appcompat.app.AppCompatActivity;

import com.jingju.jjplayer.view.JJVideoView;

public class VideoActivity extends AppCompatActivity {

    private MediaController  mMediaController;//todo 媒体控制器
    private JJVideoView mVideoView;
    private String uri ="http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/bipbop_4x3_variant.m3u8";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        /**
         * VideoView
         *      player 和 controler相关
         */

        // TODO: 2020/5/21 封装一个播放器的view

        mVideoView = findViewById(R.id.video_view);
        // TODO: 2020/5/11 获取到video path
//        mVideoView.setVideoUri(uri);








    }
}
