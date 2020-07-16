//
// Created by Macro on 2020/4/27.
//

#include "player/PlayerState.h"

void PlayerState::init() {

}


void PlayerState::setSourcePath(const char *sourcePath) {
    mSourPath=sourcePath;
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
    delete mDecoder;

}

//video 、 audio 、 subtitle
void PlayerState::initPacketQueue() {

}
