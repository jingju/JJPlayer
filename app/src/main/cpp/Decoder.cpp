//
// Created by Macro on 2020-02-14.
//
#include "Decoder.h"
#include "player/PlayerState.h"
#include <stddef.h>
#include <random>
#include "common/CommonTools.h"

#define LOG_TAG "VideoDecoder"

#include "FrameQueue2.h"

Decoder::Decoder() {
    packetQueue = new PacketQueue;
    frameQueue = new FrameQueue;
    mFrameQueue2 = new FrameQueue2;
    //todo 暂不处理
//    isPending = 0;
//    pkt = av_packet_alloc();
}

Decoder::~Decoder() {
    delete packetQueue;
    delete frameQueue;
    delete mFrameQueue2;
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