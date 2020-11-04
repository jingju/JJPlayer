//
// Created by Macro on 2020/10/20.
//

#ifndef JJPLAYER_REMUXER_H
#define JJPLAYER_REMUXER_H
extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/avassert.h"
#include "libswscale/swscale.h"
};

#include <fstream>
#include <player/PlayerState.h>
#include "player/MessageQueue.h"
#include "CommonTools.h"
#include <vector>
#include <mutex>

#define LOG_TAG "remuxer"
//音视频写入完成和错误的回调

#define VIDEO_WRITE_SUCCESS 1
#define VIDEO_WRITE_ERROR   2
#define AUDIO_WRITE_SUCCESS 3
#define AUDIO_WRITE_ERROR   4
#define END_OF_FILE   5


//todo 这里待去掉，换成传参的形式
#define STREAM_FRAME_RATE 25
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P;
#define SCALE_FLAGS SWS_BICUBIC
/**
 * 这里用一个消息队列
 */
using namespace std;
//avstream的包装
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class Remuxer {
public:
    thread videoWriteThread;
    thread audioWriteThread;
    thread messageThread;
    MessageQueue messageQueue;
    const char *videoPath = nullptr;
    const char *audioPath = nullptr;
    const char *outPutPath = nullptr;
    PlayerState *mPlayerState;
    bool isStop = false;

    AVFormatContext *fmtContext = nullptr;
    OutputStream videoOutStream = {0};
    OutputStream audioOutStream = {0};
    mutex mMutext;
    mutex mMutext2;
    bool isPendingVideo=false;//编码缓冲区未填man
    bool isPendingAudio=false;//编码缓冲区未填man
    int abortEncode=0;

public:

    Remuxer(PlayerState *playerState);

    ~Remuxer();

    int start(const char *mediaType, const char *outFileName);

    int addStream(OutputStream &outFmtContext, AVCodec **pCodec, AVCodecID id);

    void closeStream(AVFormatContext *pContext, OutputStream *pStream);


    int openVideo(AVCodec *pCodec,
                  AVDictionary *pDictionary);

    bool writeAudioFrame(AVFormatContext *fmtContext, AVFrame *frame);

    int openAudio(AVCodec *pCodec,
                  AVDictionary *pDictionary);

    bool audioWrite();

    bool videoWrite();

    void messageLoop();

    void changeState(int stateCode);

    AVMessage *getMessage();

    void setVideoPaht(const char *videoPath);

    void setAudioPath(const char *audioPath);

    void setOutputPath(const char *outPutPath);

    AVFrame *allocVideoFrame(enum AVPixelFormat pix_fmt, int width, int height);

    AVFrame *allocAudioFrame(enum AVSampleFormat sample_fmt,
                             uint64_t channel_layout,
                             int sample_rate, int nb_samples);

    bool writeVideoFrame(AVFrame *frame);

    int writeFrame(AVRational *rational, AVStream *stream,
                   AVPacket *packet);

    void writeTrailer(OutputStream &videoThream, OutputStream &audioThream,
                      int haveVideo, int haveAudio);
};


#endif //JJPLAYER_REMUXER_H
