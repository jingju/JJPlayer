//
// Created by Macro on 2020/4/27.
//

#include "player/PlayerState.h"

void PlayerState::init() {

}


void PlayerState::setSourcePath(const char *sourcePath) {
    mSourPath = sourcePath;
}


/**
 * 创建window
 * @param env
 * @param surface
 */
void PlayerState::setNativeSurface(JNIEnv *env, jobject surface) {
    _window = ANativeWindow_fromSurface(env, surface);
}

void PlayerState::changState(int stateCode) {
    AVMessage *message = new AVMessage;
    message->what = stateCode;
    mMessageQueue.push(message);//准备阶段的message
}


AVMessage *PlayerState::getMessage() {
    return mMessageQueue.wait_and_pop();
}


PlayerState::~PlayerState() {

}

//video 、 audio 、 subtitle
void PlayerState::initPacketQueue() {

}


void PlayerState::initClock(Clock *c, int *queue_serial) {
    c->speed = 1.0;
    c->paused = 0;
    c->queue_serial = queue_serial;
    setClock(c, NAN, -1);
}


void PlayerState::setClock(Clock *c, double pts, int serial) {
    //todo 微妙，需要转化成s
    double time =
            av_gettime_relative() / 1000000.0;//todo ffmpeg的时间戳 自1970年到现在的经过的时间是1479190499704705 us
    setClockAt(c, pts, serial, time);
}

void PlayerState::setClockAt(Clock *c, double pts, int serial, double time) {
    c->pts = pts;
    c->last_updated = time;
    c->pts_drift = c->pts - time;
    c->serial = serial;
}

void PlayerState::initAllClock() {
    initClock(&mVideoClock, &mVideoDecoder->packetQueue->serial);
    initClock(&mAudioClcok, &mAudioDecoder->packetQueue->serial);
    initClock(&mExctClock, &mExctClock.serial);

}

/**
 * 获取当前时钟的时间
 * @param c
 * @return
 */
double PlayerState::getClock(Clock *c) {
    if (*c->queue_serial != c->serial)
        return NAN;
    if (c->paused) {
        return c->pts;
    } else {
        double time = av_gettime_relative() / 1000000.0;
        return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
    }
}

/**
 * 获取主时钟的同步方式
 * @return
 */
int PlayerState::getMasterSyncType() {
    if (avSyncType == AV_SYNC_VIDEO_MASTER) {
        if (mVideoDecoder->stream != nullptr)
            return AV_SYNC_VIDEO_MASTER;
        else
            return AV_SYNC_AUDIO_MASTER;
    } else if (avSyncType == AV_SYNC_AUDIO_MASTER) {
        if (mAudioDecoder->stream != nullptr)
            return AV_SYNC_AUDIO_MASTER;
        else
            return AV_SYNC_EXTERNAL_CLOCK;
    } else {
        return AV_SYNC_EXTERNAL_CLOCK;
    }
}

/**
 * 设置主时钟的同步方式
 * @param syncType
 */
void PlayerState::setMasterSyncType(int syncType) {

}

/**
 * 获取主时钟的时间
 * @return
 */
double PlayerState::getMasterClock() {
    double val;
    switch (getMasterSyncType()) {
        case AV_SYNC_VIDEO_MASTER:
            val = getClock(&mVideoClock);
            break;
        case AV_SYNC_AUDIO_MASTER:
            val = getClock(&mAudioClcok);
            break;
        default:
            val = getClock(&mExctClock);
            break;
    }
    return val;
}

/**
 * 同步时钟到从动时钟
 *
 * @param c
 * @param slave
 */
void PlayerState::syncClockToSlave(Clock *c, Clock *slave) {
    double clock = getClock(c);
    double slave_clock = getClock(slave);
    if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
        setClock(c, slave_clock, slave->serial);
}

void PlayerState::setDestYuvFilePath(const char *yuvFilePath,const char *pcmFilePath) {
    mDestYuvFilePath=yuvFilePath;
    mDestPcmFilePath=pcmFilePath;

}
void PlayerState::setDestMediaPath(const char *outMediaPath) {
    mDestMedianPath=outMediaPath;
}

void PlayerState::setEndFile(int isEnd) {
    endOfFile=isEnd;
    mVideoDecoder->endOfFile=1;
    mAudioDecoder->endOfFile=1;
}
