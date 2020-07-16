//
// Created by Macro on 2020/7/13.
//

#ifndef JJPLAYER_PACKETQUEUE_H
#define JJPLAYER_PACKETQUEUE_H

#include <mutex>
#include <queue>

extern "C"{
#include "../thirdlib/ffmpeg/include/libavcodec/avcodec.h"
};

class PacketQueue {
private:
    std::mutex mMutex;
    std::condition_variable mCondition;
    std::queue<AVPacket*> mQueue;

    int mNbPackets;
    int mSize;
    int64_t duration;
    int abort_request;
public:
    PacketQueue();
    void start();
    int abort();
    void flush();
    int push(AVPacket *packet);
    int pushNullPackets(int streamIndex);
    AVPacket* waitAndPop();
    int getSize();
};


#endif //JJPLAYER_PACKETQUEUE_H
