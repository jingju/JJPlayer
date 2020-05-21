package com.jingju.jjplayer;

import com.jingju.jjplayer.annotation.AccesseByNative;

public class JJMediaPlayer {

    @AccesseByNative
    private Long mNativeMediaPlayer;

    public void nativeSetUp(){

    }

    public native void  _setUp(Object JJMediaPlayer_this);

}
