//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_AVSYNCRONIZER_H
#define JJPLAYER_AVSYNCRONIZER_H


#include <sys/types.h>
#include "VideoFrameQueue.h"
#include "AudioFrameQueue.h"
#include "AudioFrame.h"
#include "VideoFrame.h"
#include "Decoder.h"
#include <string>
#include "PlayerState.h"
#define LOG_TAG "AVSyncronizer"

using namespace std;
class AVSyncronizer {
    //todo 最后再换成重写后的decoder
private:
    PlayerState *mPlayerState= nullptr;
    VideoRenderController *mRenderController= nullptr;
    OpenSLESAudioController *mAudioController= nullptr;

    std::thread mVideoRefreshThread;//视频的刷新线程
    std::thread mReadThread;//解码后的音视频帧的读取线程

    Decoder *mAudioDecoder;
    Decoder *mVideoDecoder;
    Decoder *mSubtitleDecoder;
    std::mutex mutex;
    //todo 时钟
    int streamComponentOpen(PlayerState *playerState,AVMediaType type,int streamIndex);

public:
    AVSyncronizer(PlayerState *playerState);
    ~AVSyncronizer();
    void initDecoder();
    int readThread();
    int videoRefreshThread();
    void streamOpen();
    void createAudioRender();
    void initVideoRender();

};


#endif //JJPLAYER_AVSYNCRONIZER_H
