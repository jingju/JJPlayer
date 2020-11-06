package com.jingju.jjplayer;

import android.os.Environment;

/**
 * 读取本地数据并转封装
 *
 */
public class Remuxer {
    private String inPath= Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/playertest.mp4";
    private String outPath= Environment.getExternalStorageDirectory().getAbsolutePath()+"/playertest/output.flv";

    public Remuxer() {
    }

    public void remux(String mediaType){
        nRemux(inPath, outPath);
    }

    private native void nRemux(String inputPath, String outputPath);
}
