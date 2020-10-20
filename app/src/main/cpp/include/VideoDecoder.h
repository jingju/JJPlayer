//
// Created by Macro on 2020/10/20.
//

#ifndef JJPLAYER_VIDEODECODER_H
#define JJPLAYER_VIDEODECODER_H


#include "Decoder.h"

class VideoDecoder : public Decoder{
public:
    int width, height;//视频帧宽高
    enum AVPixelFormat pix_fmt;//图片的像素数据的格式
    uint8_t *video_dst_data[4] = {NULL};
    int      video_dst_linesize[4];
    int video_dst_bufsize; //todo  一帧缓存的大小
public:
    int decodeVideoPacket(AVPacket *pkt,AVFrame *frame);
    int videoThread();

    void stop();

    void start();
};


#endif //JJPLAYER_VIDEODECODER_H
