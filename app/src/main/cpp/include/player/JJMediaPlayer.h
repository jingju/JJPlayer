//
// Created by Macro on 2020/4/27.
//

#ifndef JJPLAYER_JJMEDIAPLAYER_H
#define JJPLAYER_JJMEDIAPLAYER_H


#include <thread>
#include <jni.h>
#include "JJPlayer.h"

//todo 定义消息队列中消息的各种状态
/**
 * 准备阶段
 */
#define MP_STATE_ASYNC_PREPARING    2


//todo jj 先搭建好框架
class JJMediaPlayer {
private:
    //消息循环线程
    JJPlayer *jjplayer;//播放器类
    std::mutex mutex;
    const char *sourcePath;

    void* weak_this;//java 类JJMediaPlayer的引用

public:

    static void message_loop();

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



};


#endif //JJPLAYER_JJMEDIAPLAYER_H
