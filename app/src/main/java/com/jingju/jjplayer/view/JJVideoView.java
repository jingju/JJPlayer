package com.jingju.jjplayer.view;
import android.annotation.TargetApi;
import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.MediaController;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.jingju.jjplayer.IMediaPlayer;
import com.jingju.jjplayer.JJMediaPlayer;

import java.io.File;
import java.io.IOException;
import java.util.Map;

public class JJVideoView extends FrameLayout implements MediaController.MediaPlayerControl {

    private SurfaceView renderView;
    private Context mContext;
    private IMediaPlayer mMediaPlayer;
    private Context mAppContext;
    private int mVideoWidth;
    private int mVideoHeight;


    // all possible internal states
    private static final int STATE_ERROR = -1;
    private static final int STATE_IDLE = 0;
    private static final int STATE_PREPARING = 1;
    private static final int STATE_PREPARED = 2;
    private static final int STATE_PLAYING = 3;
    private static final int STATE_PAUSED = 4;
    private static final int STATE_PLAYBACK_COMPLETED = 5;

    // mCurrentState is a VideoView object's current state.
    // mTargetState is the state that a method caller intends to reach.
    // For instance, regardless the VideoView object's current state,
    // calling pause() intends to bring the object to a target state
    // of STATE_PAUSED.
    private int mCurrentState = STATE_IDLE;
    private int mTargetState = STATE_IDLE;





    // All the stuff we need for playing and showing a video
    private IRenderView.ISurfaceHolder mSurfaceHolder = null;
    // private int         mAudioSession;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private int mVideoRotationDegree;
    private IMediaController mMediaController;
    private IMediaPlayer.OnCompletionListener mOnCompletionListener;
    private IMediaPlayer.OnPreparedListener mOnPreparedListener;
    private int mCurrentBufferPercentage;
    private IMediaPlayer.OnErrorListener mOnErrorListener;
    private IMediaPlayer.OnInfoListener mOnInfoListener;
    private int mSeekWhenPrepared;  // recording the seek position while preparing
    private boolean mCanPause = true;
    private boolean mCanSeekBack = true;
    private boolean mCanSeekForward = true;





    private TextView subtitleDisplay;
//    private Context mAppContext;
//    private Settings mSettings;
    private IRenderView mRenderView;
    private int mVideoSarNum;
    private int mVideoSarDen;

//    private InfoHudViewHolder mHudViewHolder;

    private long mPrepareStartTime = 0;
    private long mPrepareEndTime = 0;

    private long mSeekStartTime = 0;
    private long mSeekEndTime = 0;


    private static final int[] s_allAspectRatio = {
            IRenderView.AR_ASPECT_FIT_PARENT,
            IRenderView.AR_ASPECT_FILL_PARENT,
            IRenderView.AR_ASPECT_WRAP_CONTENT,
            // IRenderView.AR_MATCH_PARENT,
            IRenderView.AR_16_9_FIT_PARENT,
            IRenderView.AR_4_3_FIT_PARENT};
    private int mCurrentAspectRatio = s_allAspectRatio[0];
    private Uri mUri;
    private Map<String, String> mHeaders;


    public JJVideoView(Context context) {
        super(context);
        initView(context);
    }

    public JJVideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    public JJVideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context);
    }
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public JJVideoView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        initView(context);
    }

    private void initView(Context context) {
        // TODO: 2019-07-22 VideoView 初始化
        mAppContext = context.getApplicationContext();
//        mSettings = new Settings(mAppContext);

        initBackground();
        initRenders();

        mVideoWidth = 0;
        mVideoHeight = 0;
        // REMOVED: getHolder().addCallback(mSHCallback);
        // REMOVED: getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        // REMOVED: mPendingSubtitleTracks = new Vector<Pair<InputStream, MediaFormat>>();
        mCurrentState = STATE_IDLE;
        mTargetState = STATE_IDLE;

        subtitleDisplay = new TextView(context);
        subtitleDisplay.setTextSize(24);
        subtitleDisplay.setGravity(Gravity.CENTER);
        FrameLayout.LayoutParams layoutParams_txt = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.BOTTOM);
        addView(subtitleDisplay, layoutParams_txt);
    }

    // TODO: 2020/5/22 后台播放
    private void initBackground() {

    }

    // TODO: 2020/5/22 渲染引擎的初始化
    private void initRenders() {
//        mAllRenders.clear();
//
//        if (mSettings.getEnableSurfaceView())
//            mAllRenders.add(RENDER_SURFACE_VIEW);
//        if (mSettings.getEnableTextureView() && Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH)
//            mAllRenders.add(RENDER_TEXTURE_VIEW);
//        if (mSettings.getEnableNoView())
//            mAllRenders.add(RENDER_NONE);
//
//        if (mAllRenders.isEmpty())
//            mAllRenders.add(RENDER_SURFACE_VIEW);
//        mCurrentRender = mAllRenders.get(mCurrentRenderIndex);
        setRender();
    }

    // TODO: 2020/5/26 暂时写死，只有一个re 
    private void setRender() {
        SurfaceRenderView renderView = new SurfaceRenderView(getContext());
        setRenderView(renderView);
    }

    private void setRenderView(SurfaceRenderView renderView) {
        if (mRenderView != null) {
            if (mMediaPlayer != null)
                mMediaPlayer.setDisplay(null);

            View renderUIView = mRenderView.getView();
            mRenderView.removeRenderCallback(mSHCallback);
            mRenderView = null;
            removeView(renderUIView);
        }

        if (renderView == null)
            return;

        mRenderView = renderView;
        renderView.setAspectRatio(mCurrentAspectRatio);
        if (mVideoWidth > 0 && mVideoHeight > 0)
            renderView.setVideoSize(mVideoWidth, mVideoHeight);
        if (mVideoSarNum > 0 && mVideoSarDen > 0)
            renderView.setVideoSampleAspectRatio(mVideoSarNum, mVideoSarDen);

        View renderUIView = mRenderView.getView();
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER);
        renderUIView.setLayoutParams(lp);
        addView(renderUIView);

        mRenderView.addRenderCallback(mSHCallback);
        mRenderView.setVideoRotation(mVideoRotationDegree);
    }

    IRenderView.IRenderCallback mSHCallback = new IRenderView.IRenderCallback() {
        @Override
        public void onSurfaceCreated(@NonNull IRenderView.ISurfaceHolder holder, int width, int height) {
            if (holder.getRenderView() != mRenderView) {
//                Log.e(TAG, "onSurfaceCreated: unmatched render callback\n");
                return;
            }

            mSurfaceHolder = holder;
            if (mMediaPlayer != null)
                bindSurfaceHolder(mMediaPlayer, holder);
            else
                openVideo();
        }

        @Override
        public void onSurfaceChanged(@NonNull IRenderView.ISurfaceHolder holder, int format, int w, int h) {
            if (holder.getRenderView() != mRenderView) {
//                Log.e(TAG, "onSurfaceChanged: unmatched render callback\n");
                return;
            }

            mSurfaceWidth = w;
            mSurfaceHeight = h;
            boolean isValidState = (mTargetState == STATE_PLAYING);
            boolean hasValidSize = !mRenderView.shouldWaitForResize() || (mVideoWidth == w && mVideoHeight == h);
            if (mMediaPlayer != null && isValidState && hasValidSize) {
                if (mSeekWhenPrepared != 0) {
                    seekTo(mSeekWhenPrepared);
                }
                start();
            }
        }

        @Override
        public void onSurfaceDestroyed(@NonNull IRenderView.ISurfaceHolder holder) {

        }
    };

    //todo player 和 holder 绑定，同时创建surface 相关

    private void bindSurfaceHolder(IMediaPlayer mp, IRenderView.ISurfaceHolder holder) {
        if (mp == null)
            return;

        if (holder == null) {
            mp.setDisplay(null);
            return;
        }

        holder.bindToMediaPlayer(mp);
    }

    public void setVideoPath(String path){
        setVideoURI(Uri.parse(path));
    }
    public void setVideoURI(Uri uri, Map<String,String> headers){
        mUri = uri;
        mHeaders = headers;
        mSeekWhenPrepared = 0;
        openVideo();
        requestLayout();
        invalidate();
    }
    public void setVideoURI(Uri uri){
        setVideoURI(uri,null);
    }
    /**
     * todo 开始播放相关操作
     *
     */
    public void openVideo(){
        if (mUri == null || mSurfaceHolder == null) {
            // not ready for playback just yet, will try again later
            return;
        }
//        // we shouldn't clear the target state, because somebody might have
//        // called start() previously
//        release(false);

        AudioManager am = (AudioManager) mAppContext.getSystemService(Context.AUDIO_SERVICE);
        am.requestAudioFocus(null, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);

        try {
            // TODO: 2020-02-24 创建播放器
//            mMediaPlayer = createPlayer(mSettings.getPlayer());
            mMediaPlayer = createPlayer();
            // TODO: create SubtitleController in MediaPlayer, but we need
            // a context for the subtitle renderers
            final Context context = getContext();
            // REMOVED: SubtitleController

            // REMOVED: mAudioSession
            // TODO: 2020/5/26  
//            mMediaPlayer.setOnPreparedListener(mPreparedListener);
//            mMediaPlayer.setOnVideoSizeChangedListener(mSizeChangedListener);
//            mMediaPlayer.setOnCompletionListener(mCompletionListener);
//            mMediaPlayer.setOnErrorListener(mErrorListener);
//            mMediaPlayer.setOnInfoListener(mInfoListener);
//            mMediaPlayer.setOnBufferingUpdateListener(mBufferingUpdateListener);
//            mMediaPlayer.setOnSeekCompleteListener(mSeekCompleteListener);
//            mMediaPlayer.setOnTimedTextListener(mOnTimedTextListener);
            mCurrentBufferPercentage = 0;
            // TODO: 2020/5/26  
            String scheme = mUri.getScheme();
            // TODO: 2020/5/27 dataSource 暂时不适配，写死。
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M &&
//                    mSettings.getUsingMediaDataSource() &&
//                    (TextUtils.isEmpty(scheme) || scheme.equalsIgnoreCase("file"))) {
//                IMediaDataSource dataSource = new FileMediaDataSource(new File(mUri.toString()));
//                mMediaPlayer.setSourcePath(dataSource);
//            }  else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH) {
//                mMediaPlayer.setSourcePath(mAppContext, mUri, mHeaders);
//            } else {
                mMediaPlayer.setDataSource(mUri.toString());
//            }
            bindSurfaceHolder(mMediaPlayer, mSurfaceHolder);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.setScreenOnWhilePlaying(true);
            mPrepareStartTime = System.currentTimeMillis();// TODO: 2020-02-24 准备播放开始时间
            mMediaPlayer.prepareAsync();
//            if (mHudViewHolder != null)
//                mHudViewHolder.setMediaPlayer(mMediaPlayer);

            // REMOVED: mPendingSubtitleTracks

            // we don't set the target state here either, but preserve the
            // target state that was there before.
//            mCurrentState = STATE_PREPARING;
//            attachMediaController();
        } catch (IOException ex) {
//            Log.w(TAG, "Unable to open content: " + mUri, ex);
//            mCurrentState = STATE_ERROR;
//            mTargetState = STATE_ERROR;
//            mErrorListener.onError(mMediaPlayer, MediaPlayer.MEDIA_ERROR_UNKNOWN, 0);
        } catch (IllegalArgumentException ex) {
//            Log.w(TAG, "Unable to open content: " + mUri, ex);
//            mCurrentState = STATE_ERROR;
//            mTargetState = STATE_ERROR;
//            mErrorListener.onError(mMediaPlayer, MediaPlayer.MEDIA_ERROR_UNKNOWN, 0);
        } finally {
             //REMOVED: mPendingSubtitleTracks.clear();
        }

    }

    // TODO: 2020/5/27 暂时写死，只有一种，不适配
    private IMediaPlayer createPlayer() {
        return new JJMediaPlayer();
    }


    //播放器控制相关回调
    @Override
    public void start() {

    }

    @Override
    public void pause() {

    }

    @Override
    public int getDuration() {
        return 0;
    }

    @Override
    public int getCurrentPosition() {
        return 0;
    }

    @Override
    public void seekTo(int pos) {

    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public int getBufferPercentage() {
        return 0;
    }

    @Override
    public boolean canPause() {
        return false;
    }

    @Override
    public boolean canSeekBackward() {
        return false;
    }

    @Override
    public boolean canSeekForward() {
        return false;
    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }
}
