//
// Created by Macro on 2020/6/19.
//

#ifndef JJPLAYER_OPENSLESAUDIOCONTROLLER_H
#define JJPLAYER_OPENSLESAUDIOCONTROLLER_H
extern "C" {
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

#include <cstddef>
#include <malloc.h>
#include <jni.h>
#include <mutex>
#include <libavutil/samplefmt.h>
#include "BaseAudioController.h"
#include "thread"

#define OPENSLES_BUFFERS 4 // 最大缓冲区数量
#define OPENSLES_BUFLEN  10 // 缓冲区长度(毫秒)
/**
 * todo 为了可以使用不同的方式，到时候抽取一个父类。
 */
//static SLmilliHertz bqPlayerSampleRate = 0;
//static SLObjectItf outputMixObject = nullptr;
//static SLEngineItf engineEngine;
//static SLObjectItf bqPlayerObject = nullptr;
//static SLPlayItf bqPlayerPlay;
//static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
//static SLEffectSendItf bqPlayerEffectSend;
//static SLVolumeItf bqPlayerVolume;
//static short *nextBuffer;
//static unsigned nextSize;
//static int nextCount;


class OpenSLESAudioController : public BaseAudioController {
private:

     SLmilliHertz bqPlayerSampleRate = 0;
     SLObjectItf outputMixObject = nullptr;
     SLEngineItf engineEngine;
     SLObjectItf bqPlayerObject = nullptr;
     SLPlayItf bqPlayerPlay;
     SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
     SLEffectSendItf bqPlayerEffectSend;
     SLVolumeItf bqPlayerVolume;
     short *nextBuffer;
     unsigned nextSize;
     int nextCount;
// engine interfaces
    SLObjectItf engineObject = nullptr;
//    static jint   bqPlayerBufSize = 0;

    // buffer queue player interfaces

    SLMuteSoloItf bqPlayerMuteSolo;

    AudioDeviceSpec audioDeviceSpec;    // 音频设备参数
    int bytes_per_frame;                // 一帧占多少字节
    int milli_per_buffer;               // 一个缓冲区时长占多少
    int frames_per_buffer;              // 一个缓冲区有多少帧
    int bytes_per_buffer;               // 一个缓冲区的大小
    uint8_t *buffer;                    // 缓冲区
    size_t buffer_capacity;             // 缓冲区总大小

    jint bqPlayerBufSize = 0;
    short *resampleBuf = NULL;
    std::mutex mMutex;
    std::thread *audioPlayThread;//音频播放线程

// pointer and size of the next player buffer to enqueue, and number of remaining buffers


    int abortRequest;                   // 终止标志
    int pauseRequest;                   // 暂停标志
    int flushRequest;

public:
    int init(const AudioDeviceSpec *desired, AudioDeviceSpec *obtained) override;

    void start() override;

    void resume() override;

    void pause() override;


    //todo 抽取父类的时候，改成createPlayer;
    static void createBufferQueueAudioPlayer(int sampleRate, int bufferSize);

    void releaseResampleBuf();

    SLuint32 getSLSampleRate(int sampleRate);

    int prepareBufferQueue();


};


#endif //JJPLAYER_OPENSLESAUDIOCONTROLLER_H
