//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_AVSYNCRONIZER_H
#define JJPLAYER_AVSYNCRONIZER_H


#include <sys/types.h>
#include "Decoder.h"
#include "VideoFrameQueue.h"
#include "AudioFrameQueue.h"
#include "AudioFrame.h"
#include "VideoFrame.h"
#include <string>

using namespace std;
class AVSyncronizer {
private:
    Decoder videoDecoder;
    VideoFrameQueue videoFrameQueue;
    AudioFrameQueue audioFrameQueue;
    pthread_t decoderThread;

private:
    void run();
public:
    //todo 参数准确性待确认
    int start(string  uri);
    AudioFrame getAudioFrame();
    VideoFrame getCorrectVideoFrame();
    void stop();
};


#endif //JJPLAYER_AVSYNCRONIZER_H
