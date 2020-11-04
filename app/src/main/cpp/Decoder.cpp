//
// Created by Macro on 2020-02-14.
//
#include "Decoder.h"
#include "player/PlayerState.h"
#include <stddef.h>
#include <random>
#define LOG_TAG "VideoDecoder"

#include "FrameQueue2.h"
Decoder::Decoder() {
    packetQueue = new PacketQueue;
    mDecodeFrameQueue2 = new FrameQueue2;
    mEncodeFrameQueue=new AVFrameQueue;
    //todo 暂不处理
//    isPending = 0;
//    pkt = av_packet_alloc();
}

Decoder::~Decoder() {
    delete packetQueue;
    delete mDecodeFrameQueue2;
    delete mEncodeFrameQueue;
}

/** connect time out call back*/
//int Decoder::interrupt_cb(void *ctx) {
//    //todo 解码失败的重试
//
//}

/** release resource */
void Decoder::release() {

}

void Decoder::init(AVCodecContext *codecContext, const char *destDataFilePath) {
    this->codecContext = codecContext;
    this->destDataFilePath = destDataFilePath;
}

/**
 * 停止
 */
void Decoder::stop() {


}