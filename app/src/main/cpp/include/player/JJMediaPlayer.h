//
// Created by Macro on 2020/4/27.
//

#ifndef JJPLAYER_JJMEDIAPLAYER_H
#define JJPLAYER_JJMEDIAPLAYER_H


#include <thread>
#include <jni.h>
#include "AVSyncronizer.h"
#define LOG_TAG "JJMediaPlayer"
//todo 定义消息队列中消息的各种状态
/**
 * 准备阶段
 */
#define MP_STATE_ASYNC_PREPARING    2

class JJMediaPlayer {
public:
    //消息循环线程
    PlayerState *mPlayerState;//播放器类
    AVSyncronizer *mAVSyncronizer;//音视频的同步类

    std::mutex mutex;
    void* weak_this;//java 类JJMediaPlayer的引用
    std::thread mThreadMessageLoop;
public:

    JJMediaPlayer();
    ~JJMediaPlayer();
    void message_loop();

    void setNativieSurface(JNIEnv *env,jobject surface);
    /**
     * 设置资源放来源
     */
    void setDataSource(JNIEnv *env,jstring path, jobjectArray keys, jobjectArray values);

    //准备阶段
    void prepareAsync();

    void prepareAsyncl();

    //开始播放
    void start();//总的开始的方法

    void setJavaJJPlayerRef(void *ref);


    void setDestFilePath(JNIEnv *env, jstring destYuvPath,jstring destPcmPath);
};


#endif //JJPLAYER_JJMEDIAPLAYER_H
