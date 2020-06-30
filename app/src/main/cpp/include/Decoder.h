//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_DECODER_H
#define JJPLAYER_DECODER_H

#include <string>
#include <thread>
#include <videorender/VideoRenderController.h>
#include <audiorender/AudioResampler.h>
#include "audiorender/BaseAudioController.h"

#include "audiorender/OpenSLESAudioController.h"
#include "PlayState.h"
#include "FrameQueue.h"

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
    VideoRenderController *mRenderController;
    OpenSLESAudioController *mAudioController;
    AudioResampler *audioResampler;
//    void* protocalParser;//解协议
//    void* formatDemuxer;//解封装

    int ret=0;
    int got_frame;
    AVFormatContext* fmt_ctx;

    int video_stream_idx = -1;
    int audio_stream_idx = -1;
    AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
    AVStream *video_stream = NULL, *audio_stream = NULL;
    int refcount=0;

    int width, height;//视频帧宽高
    enum AVPixelFormat pix_fmt;//图片的像素数据的格式
    uint8_t *video_dst_data[4] = {NULL};
    int      video_dst_linesize[4];
    int video_dst_bufsize; //todo  一帧缓存的大小
    AVFrame *frame = NULL;
    AVPacket pkt;
//    void* mDecoder;//解码
    thread mVideoThead;
    thread mAudioThread;
    FrameQueue *mVideoFrameQueue;
    FrameQueue *mAudioFrameQueue;

public:
    Decoder();
    int openFile(const char *src_filename,VideoRenderController *controller,OpenSLESAudioController *audioController);
    void closeFile();
    bool isEOF();
    int decode();


    int interrupt_cb(void *ctx);

    int  open_codec_context(AVCodecContext *dec_ctx,  AVStream * st ,AVMediaType type);
    int decode_packet(int *got_frame, int cached);
    int decodeVideoPacket(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);

    void videoThread();
    void audioThread();
    void release();

    int openAudioController(int64_t wanted_channel_layout, int wanted_nb_channels,
                             int wanted_sample_rate);

};


#endif //JJPLAYER_DECODER_H
