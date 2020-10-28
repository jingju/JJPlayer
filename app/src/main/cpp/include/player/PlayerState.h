//
// Created by Macro on 2020/4/27.
//

#ifndef JJPLAYER_PLAYERSTATE_H
#define JJPLAYER_PLAYERSTATE_H
#define VIDEO_QUEUE_SIZE 3
#define SAMPLE_QUEUE_SIZE 9

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25

#define AUDIO_MIN_BUFFER_SIZE 512

#define AUDIO_MAX_CALLBACKS_PER_SEC 30

#define REFRESH_RATE 0.01

#define AV_SYNC_THRESHOLD_MIN 0.04

#define AV_SYNC_THRESHOLD_MAX 0.1

#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1

#define AV_NOSYNC_THRESHOLD 10.0


#define EXTERNAL_CLOCK_MIN_FRAMES 2

#define EXTERNAL_CLOCK_MAX_FRAMES 10

#define EXTERNAL_CLOCK_SPEED_MIN  0.900

#define EXTERNAL_CLOCK_SPEED_MAX  1.010

#define EXTERNAL_CLOCK_SPEED_STEP 0.001

#define AUDIO_DIFF_AVG_NB   20

#define SAMPLE_CORRECTION_PERCENT_MAX 10


#include "VideoOutput.h"
#include "AudioOutput.h"
#include "MessageQueue.h"
#include "CommonTools.h"
#include <jni.h>
#include <android/native_window_jni.h>
#include <thread>
#include <PacketQueue.h>
#include "Decoder.h"
#include "videorender/VideoRenderController.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"

extern "C"{
#include <libavutil/time.h>
}
typedef struct Clock{
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int *queue_serial;

} Clock;

enum {
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

class PlayerState {

public:
    MessageQueue mMessageQueue;
    const char * mSourPath;

    //todo 抽象成这样两个对象
    VideoOutput *videoOutput;
    AudioOutput *audioOutput;
    ANativeWindow *_window;


    AudioDecoder *mAudioDecoder= nullptr;
    VideoDecoder *mVideoDecoder= nullptr;
    Decoder *mSubtitleDecoder;

    //packetQueue都放在了decoder里面。
//    PacketQueue  mVideQueue;
//    PacketQueue  mAudioQueue;
//    PacketQueue  mSubTitleQueue;
    //todo subtitle queue 字幕队列





    //todo 时钟
    Clock mAudioClcok;//音频时钟
    Clock mVideoClock;//视频时钟
    Clock mExctClock;//外部时钟
    int avSyncType=AV_SYNC_VIDEO_MASTER;//时钟的同步方式
//    int avSyncType=AV_SYNC_AUDIO_MASTER;//时钟的同步方式//todo 暂时写成这种方式，测试显示
    double frameTimer=0.0;

    AVFormatContext *mFormtContext= nullptr;
    int lowres=0;//低分辨率解码
    int fast;              // 解码上下文的AV_CODEC_FLAG2_FAST标志


    int64_t startTime;              // 播放起始位置
    int64_t duration;               // 播放时长    //todo 默认是 NO_OPTION_VALUE



    //todo
    uint8_t * audioBuffer;//音频缓冲区指针

    //todo 相关的初始化操作

    int abort_request=0;//todo 停止刷新


    const char *mDestYuvFilePath= nullptr;
    const char *mDestPcmFilePath= nullptr;
public:
    /**
     *
     */
     ~PlayerState();

    void init();

    void setNativeSurface(JNIEnv *env, jobject surface);

    void changState(int stateCode);

    AVMessage *getMessage();

    void setSourcePath(const char * sourcePath);

    void initPacketQueue();

    void initClock(Clock *c, int *queue_serial);
    void setClockAt(Clock *c, double pts, int serial, double time);
    void setClock(Clock *c, double pts, int serial);
    void initAllClock();

    double getClock(Clock *c);
    int getMasterSyncType();
    void setMasterSyncType(int syncType);
    double getMasterClock();
    void syncClockToSlave(Clock *c ,Clock *slave);

    void setDestYuvFilePath(const char *yuvPath,const char *pcmPath);

};


#endif //JJPLAYER_PLAYERSTATE_H
