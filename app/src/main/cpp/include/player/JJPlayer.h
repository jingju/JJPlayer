//
// Created by Macro on 2020/4/27.
//

#ifndef JJPLAYER_JJPLAYER_H
#define JJPLAYER_JJPLAYER_H


#include <VideoOutput.h>
#include <AudioOutput.h>
#include <AVSyncronizer.h>
#include "MessageQueue.h"

class JJPlayer {

private:
    MessageQueue messageQueue;
    VideoOutput *videoOutput;
    AudioOutput *audioOutput;
    //音视频的同步来的类
    AVSyncronizer * syncronizer;

public:
    //todo 相关的初始化操作
    /**
     *
     */
    void init();
    void changState(int stateCode);
    //player 的prepareAsync方法。
    void prepareAsync();


    int repareAsyncl();

    void streamOpen(char * fileName);

    static int videoRefreshThread();
    static int readThread();
    static int streamComponentOpen();
    static int audioThread();
    static int videoThread();
    static int subtitleThread();




};


#endif //JJPLAYER_JJPLAYER_H
