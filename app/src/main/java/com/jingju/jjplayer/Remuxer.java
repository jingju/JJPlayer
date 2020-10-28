package com.jingju.jjplayer;

import android.os.Environment;

/**
 * 读取本地数据并转封装
 *
 */
public class Remuxer {
    private String outPath= Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/out.mp4";
    public Remuxer() {
    }

    public void remux(String mediaType){
        nRemux(mediaType,outPath);
    }

    private   native void  nRemux(String mediaType,String outputPath);
}
