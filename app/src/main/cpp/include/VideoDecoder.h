//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_VIDEODECODER_H
#define JJPLAYER_VIDEODECODER_H

#include <string>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
};

using namespace std;
class VideoDecoder {
private:
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
    int video_dst_bufsize;
    AVFrame *frame = NULL;
    AVPacket pkt;
//    void* decoder;//解码

public:

    int openFile(char *src_filename);
    void closeFile();
    bool isEOF();
    int decodeFrame(float duration);


    int interrupt_cb(void *ctx);

    int  open_codec_context(int *stream_idx,
                                  AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
    int decode_packet(int *got_frame, int cached);
    int decodeVideoPacket(int *got_frame,AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);

    void release();
};


#endif //JJPLAYER_VIDEODECODER_H
