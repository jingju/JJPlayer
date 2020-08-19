//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_DECODER_H
#define JJPLAYER_DECODER_H

#include <string>
#include <thread>
#include "FrameQueue.h"
#include "PacketQueue.h"

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
    AVStream *video_stream = NULL, *audio_stream = NULL;
    int refcount=0;

    int width, height;//视频帧宽高
    enum AVPixelFormat pix_fmt;//图片的像素数据的格式
    uint8_t *video_dst_data[4] = {NULL};
    int      video_dst_linesize[4];
    int video_dst_bufsize; //todo  一帧缓存的大小
    AVFrame *aFrame = NULL;
    AVFrame *vFrame=NULL;
    AVPacket pkt;
//    void* mDecoder;//解码
    thread mVideoThead;
    thread mAudioThread;
    FrameQueue *mVideoFrameQueue;
    FrameQueue *mAudioFrameQueue;



    //todo=====new ===== 删除上面多余的
    AVCodecContext *codecContext;
    PacketQueue  *packetQueue;
    std::thread decodeThread;
    FrameQueue *frameQueue;

    bool  abortRequest;//停止解码

public:
    Decoder();
    ~Decoder();
    void init(AVCodecContext *codecContext);
    void closeFile();
    bool isEOF();


    int interrupt_cb(void *ctx);


    int videoThread();
    int audioThread();
    void release();



    //========todo new ======
    void start(AVMediaType type);
    void stop();
    int decodePacketToFrame(AVPacket *pkt,AVFrame *frame);


};


#endif //JJPLAYER_DECODER_H
