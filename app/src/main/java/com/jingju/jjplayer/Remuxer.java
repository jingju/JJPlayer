package com.jingju.jjplayer;

/**
 * 读取本地数据并转封装
 *
 */
public class Remuxer {
    public Remuxer() {
    }

    public void remux(String mediaType){
        nRemux(mediaType);
    }

    public  native void  nRemux(String mediaType);
}
