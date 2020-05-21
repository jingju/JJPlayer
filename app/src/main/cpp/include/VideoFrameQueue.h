//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_VIDEOFRAMEQUEUE_H
#define JJPLAYER_VIDEOFRAMEQUEUE_H


#include "VideoFrame.h"

class VideoFrameQueue {
private:
    int size;
    VideoFrame *header;
public:
    void push(VideoFrame* frame);
    int pop(VideoFrame* frame);

};


#endif //JJPLAYER_VIDEOFRAMEQUEUE_H
