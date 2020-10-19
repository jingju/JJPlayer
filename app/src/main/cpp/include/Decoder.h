//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_DECODER_H
#define JJPLAYER_DECODER_H

#include <string>
#include <thread>
#include "FrameQueue.h"
#include "PacketQueue.h"
#include <FrameQueue2.h>
#include <fstream>
#include <iostream>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
};

//todo 最后不要忘了错误处理的时候，释放资源。
/**
 * todo  解码器
 *
 *  av_frame_free(&frame);
    avcodec_free_context(&c);
    av_packet_free(&pkt);
 */
using namespace std;
/**
 *  todo 最后抽象一个父类，将video decoder 和 audio decoder分开
 */
class Decoder {
public:
    std::mutex mutex;
//    void* protocalParser;//解协议
//    void* formatDemuxer;//解封装

    int ret=0;
    int got_frame;
    AVFormatContext* fmt_ctx;

    int video_stream_idx = -1;
    int audio_stream_idx = -1;
    int subtitle_stream_idx = -1;
    AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
    AVStream *video_stream = nullptr, *audio_stream = nullptr;
    int refcount=0;

    int width, height;//视频帧宽高
    enum AVPixelFormat pix_fmt;//图片的像素数据的格式
    uint8_t *video_dst_data[4] = {NULL};
    int      video_dst_linesize[4];
    int video_dst_bufsize; //todo  一帧缓存的大小
    AVFrame *aFrame = NULL;//todo 待换成nullptr
    AVFrame *vFrame=NULL;
    AVPacket *pkt= nullptr;//全局使用的AVPacket;
//    void* mDecoder;//解码
    //todo=====new ===== 删除上面多余的
    AVCodecContext *codecContext;
    PacketQueue  *packetQueue;
    std::thread decodeThread;
    FrameQueue *frameQueue;

    bool  abortRequest;//停止解码
    bool  isPending=false;



    //todo 新加的转换后的统一的
    FrameQueue2* mFrameQueue2;

    //audio
    int64_t  next_pts=0;
    AVRational next_pts_tb;//时间基



   //裸数据保存的位置
   const char * destDataFilePath= nullptr;


public:
    Decoder();
    ~Decoder();
    void init(AVCodecContext *codecContext,const char * destDataSavePath);
    void closeFile();
    bool isEOF();


    int interrupt_cb(void *ctx);


    int videoThread();
    int audioThread();
    void release();



    //========todo new ======
    void start(AVMediaType type);
    void stop();
    int decodePacketToFrame(AVPacket *pkt,AVFrame *frame,ofstream &out);


};


#endif //JJPLAYER_DECODER_H
