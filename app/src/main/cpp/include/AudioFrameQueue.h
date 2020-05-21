//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_AUDIOFRAMEQUEUE_H
#define JJPLAYER_AUDIOFRAMEQUEUE_H


#include "AudioFrame.h"

class AudioFrameQueue {

private:
    int size;
    AudioFrame* header;
public:
    void init();
    void push(AudioFrame* frame);
    int pop(AudioFrame* frame, bool block);
};


#endif //JJPLAYER_AUDIOFRAMEQUEUE_H
