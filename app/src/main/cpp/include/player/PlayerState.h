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
class PlayerState {

public:
    MessageQueue mMessageQueue;
    Decoder * mDecoder;
    const char * mSourPath;

    //todo 抽象成这样两个对象
    VideoOutput *videoOutput;
    AudioOutput *audioOutput;
    ANativeWindow *_window;


    Decoder *mAudioDecoder;
    Decoder *mVideoDecoder;
    Decoder *mSubtitleDecoder;

    PacketQueue  mVideQueue;
    PacketQueue  mAudioQueue;
    PacketQueue  mSubTitleQueue;
    //todo subtitle queue 字幕队列






    AVFormatContext *mFormtContext= nullptr;
    int lowres=0;//低分辨率解码
    int fast;              // 解码上下文的AV_CODEC_FLAG2_FAST标志


    int64_t startTime;              // 播放起始位置
    int64_t duration;               // 播放时长    //todo 默认是 NO_OPTION_VALUE



    //todo
    uint8_t * audioBuffer;//音频缓冲区指针

public:
    //todo 相关的初始化操作
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

};


#endif //JJPLAYER_PLAYERSTATE_H
