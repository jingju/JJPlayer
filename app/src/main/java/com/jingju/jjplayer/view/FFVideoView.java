package com.jingju.jjplayer.view;
import android.content.Context;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import com.jingju.jjplayer.IMediaPlayer;

public class FFVideoView extends FrameLayout {

    private SurfaceView renderView;
    private Context mContext;
    private IMediaPlayer mMediaoPlayer;

    public FFVideoView(Context context) {
        super(context);
        mContext=context;
        initView();
    }

    private void initView() {
        renderView = new SurfaceView(mContext);
        renderView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Surface surface = holder.getSurface();
                // TODO: 2020/5/12 初始化渲染的surface
                //  创建完surface在去解码


            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    /**
     * 设置视频的地址
     */
    public void setVideoUri(String uri){

        openVideo();
    }
    /**
     * todo 开始播放相关操作
     *
     */
    public void openVideo(){


    }


}
