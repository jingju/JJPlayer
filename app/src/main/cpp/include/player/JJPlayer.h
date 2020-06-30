//
// Created by Macro on 2020/4/27.
//

#ifndef JJPLAYER_JJPLAYER_H
#define JJPLAYER_JJPLAYER_H


#include <VideoOutput.h>
#include <AudioOutput.h>
#include <AVSyncronizer.h>
#include "MessageQueue.h"
#include "CommonTools.h"
#include <jni.h>
#include <android/native_window_jni.h>
#include <thread>
#include <videorender/VideoRenderController.h>

class JJPlayer {

private:
    MessageQueue mMessageQueue;
    Decoder * mDecoder;
    VideoRenderController *mRenderController;
    OpenSLESAudioController *mAudioController;
    const char * mSourPath;
    VideoOutput *videoOutput;
    AudioOutput *audioOutput;
    //音视频的同步来的类
    AVSyncronizer *syncronizer;
    ANativeWindow *_window;
    std::thread mVideoRefreshThread;//视频的刷新线程
    std::thread mReadThread;//解码后的音视频帧的读取线程

public:
    //todo 相关的初始化操作
    /**
     *
     */
     ~JJPlayer();
    void init();

    void setNativeSurface(JNIEnv *env, jobject surface);

    void changState(int stateCode);

    AVMessage *getMessage();

    //player 的prepareAsync方法。
    void prepareAsync();

    void setSourcePath(const char * sourcePath);
    int repareAsyncl();

    void streamOpen();

    int videoRefreshThread();

    int readThread();

    int streamComponentOpen();

    int audioThread();

    int videoThread();

    int subtitleThread();

    void initDecoder();
    void initVideoRender();
    void createAudioRender();

};


#endif //JJPLAYER_JJPLAYER_H