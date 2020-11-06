package com.jingju.jjplayer;

import android.content.Context;
import android.net.Uri;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.jingju.jjplayer.annotation.AccesseByNative;
import com.jingju.jjplayer.view.MediaInfo;

import java.io.FileDescriptor;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Map;

public class JJMediaPlayer implements IMediaPlayer{

    @AccesseByNative
    private Long mNativeMediaPlayer;

    private SurfaceHolder mSurfaceHolder;
    private String mDataSource;

    public JJMediaPlayer() {
        initPlayer();
    }

    private void initPlayer() {
        // TODO: 2020/5/27 还有一些其它的初始化工作
        setUp(new WeakReference<JJMediaPlayer>(this));
    }

    public void nativeSetUp(){

    }

    public native void setUp(Object JJMediaPlayer_this);

    @Override
    public void setDisplay(SurfaceHolder sh) {
        mSurfaceHolder = sh;
        Surface surface;
        if (sh != null) {
            surface = sh.getSurface();
        } else {
            surface = null;
        }
        nSetVideoSurface(surface);
//        updateSurfaceScreenOn();
    }

    @Override
    public void setDataSource(Context context, Uri uri) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    @Override
    public void setDataSource(Context context, Uri uri, Map<String, String> headers) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    @Override
    public void setDataSource(FileDescriptor fd) throws IOException, IllegalArgumentException, IllegalStateException {

    }

    @Override
    public void setDataSource(String path) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {
        mDataSource = path;
        nSetDataSource(path, null, null);
    }


    private native void nSetDataSource(String path,String[] keys,String[] values);

    @Override
    public String getDataSource() {
        return null;
    }

    // TODO: 2020/5/27 开始前的一些准备工作
    @Override
    public void prepareAsync() throws IllegalStateException {
        nPrepareAsync();
    }

    @Override
    public void start() throws IllegalStateException {

    }

    @Override
    public void stop() throws IllegalStateException {

    }

    @Override
    public void pause() throws IllegalStateException {

    }

    @Override
    public void setScreenOnWhilePlaying(boolean screenOn) {

    }

    @Override
    public int getVideoWidth() {
        return 0;
    }

    @Override
    public int getVideoHeight() {
        return 0;
    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public void seekTo(long msec) throws IllegalStateException {

    }

    @Override
    public long getCurrentPosition() {
        return 0;
    }

    @Override
    public long getDuration() {
        return 0;
    }

    @Override
    public void release() {

    }

    @Override
    public void reset() {

    }

    @Override
    public void setVolume(float leftVolume, float rightVolume) {

    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }

    @Override
    public MediaInfo getMediaInfo() {
        return null;
    }

    @Override
    public void setLogEnabled(boolean enable) {

    }

    @Override
    public boolean isPlayable() {
        return false;
    }

    @Override
    public void setOnPreparedListener(OnPreparedListener listener) {

    }

    @Override
    public void setOnCompletionListener(OnCompletionListener listener) {

    }

    @Override
    public void setOnBufferingUpdateListener(OnBufferingUpdateListener listener) {

    }

    @Override
    public void setOnSeekCompleteListener(OnSeekCompleteListener listener) {

    }

    @Override
    public void setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener) {

    }

    @Override
    public void setOnErrorListener(OnErrorListener listener) {

    }

    @Override
    public void setOnInfoListener(OnInfoListener listener) {

    }

    @Override
    public void setAudioStreamType(int streamtype) {

    }

    @Override
    public void setKeepInBackground(boolean keepInBackground) {

    }

    @Override
    public int getVideoSarNum() {
        return 0;
    }

    @Override
    public int getVideoSarDen() {
        return 0;
    }

    @Override
    public void setWakeMode(Context context, int mode) {

    }

    @Override
    public void setLooping(boolean looping) {

    }

    @Override
    public boolean isLooping() {
        return false;
    }

    @Override
    public void setSurface(Surface surface) {

    }

    @Override
    public void setDestFilePath(String yuvPath, String pcmPath) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {
        mSetDestYuvPath(yuvPath, pcmPath);
    }

    public native void nSetVideoSurface(Surface surface);

    public native void nPrepareAsync();


    public native void mSetDestYuvPath(String yuvPath,String pcmPath);


}
