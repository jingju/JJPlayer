//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_DECODER_H
#define JJPLAYER_DECODER_H

#include <string>
#include <thread>
#include "PacketQueue.h"
#include <FrameQueue2.h>
#include <fstream>
#include <iostream>
#include "CommonTools.h"
#include "AVFrameQueue.h"
extern "C" {
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

    int ret = 0;
    int got_frame;
    AVFormatContext *fmtContext;

    int streamIndex = -1;

    AVCodecContext *codecContext = nullptr;
    AVStream *stream = nullptr;
    int refcount = 0;

    AVFrame *aFrame = NULL;//todo 待换成nullptr
    AVFrame *vFrame = NULL;
    AVPacket *pkt = nullptr;//全局使用的AVPacket;
//    void* mDecoder;//解码
    //todo=====new ===== 删除上面多余的
    PacketQueue *packetQueue;
    std::thread decodeThread;

    bool abortRequest;//停止解码


    //todo 新加的转换后的统一的
    FrameQueue2 *mDecodeFrameQueue2;

    //audio
    int64_t next_pts = 0;
    AVRational next_pts_tb;//时间基



    //裸数据保存的位置
    const char *destDataFilePath = nullptr;

    ofstream outFileStream;


    AVFrameQueue *mEncodeFrameQueue;
    int endOfFile=0;

public:
    Decoder();

    ~Decoder();

    void init(AVCodecContext *codecContext, const char *destDataSavePath);

    void closeFile();

    bool isEOF();

    int interrupt_cb(void *ctx);

    void release();

    //========todo new ======
    virtual void start() = 0;

    virtual void stop() = 0;


};


#endif //JJPLAYER_DECODER_H
