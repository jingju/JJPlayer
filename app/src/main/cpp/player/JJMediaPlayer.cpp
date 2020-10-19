//
// Created by Macro on 2020/4/27.
//

#include "player/JJMediaPlayer.h"


JJMediaPlayer::JJMediaPlayer() {
    mPlayerState = new PlayerState;
    mAVSyncronizer = new AVSyncronizer(mPlayerState);
}

void JJMediaPlayer::prepareAsync() {
    mutex.lock();
    prepareAsyncl();
    mutex.unlock();
}


void JJMediaPlayer::prepareAsyncl() {
    // 消息队列初始化，并放入第一个消息
    mPlayerState->changState(MP_STATE_ASYNC_PREPARING);
//        //todo 开启消息循环的线程
    //todo c++每个成员函数都有一个形参，隐藏的this指针。
    mThreadMessageLoop = std::thread(&JJMediaPlayer::message_loop,this);
    //必须加detach或join
    mThreadMessageLoop.detach();


//    if (!videoOutput) {
//        return -1;
//    }
    mAVSyncronizer->streamOpen();

    /**
     * todo 后面的一些操作待定
     */
}

/**
 * todo 循环获取接收到的消息
 */
void JJMediaPlayer::message_loop() {
    while (true) {
        //todo 读取消息
        AVMessage *message = mPlayerState->getMessage();
        int status = message->what;
        switch (status) {
            case MP_STATE_ASYNC_PREPARING:
                    LOGI("on get message prepare");
                break;
        }
    }

}


void JJMediaPlayer::setJavaJJPlayerRef(void *ref) {
    weak_this = ref;
}


void JJMediaPlayer::setNativieSurface(JNIEnv *env, jobject surface) {
    if (NULL != mPlayerState) {
        mPlayerState->setNativeSurface(env, surface);
    }
}


/**
 * 设置资源的路径
 * @param env
 * @param path
 * @param keys
 * @param values
 */
void JJMediaPlayer::setDataSource(JNIEnv *env, jstring path, jobjectArray keys,
                                  jobjectArray values) {
    const char * sourcePath = env->GetStringUTFChars(path, NULL);
    mPlayerState->setSourcePath(sourcePath);
}

void JJMediaPlayer::setDestFilePath(JNIEnv *env, jstring destYuvPath,jstring destPcmPath) {
    const char * yuvPath = env->GetStringUTFChars(destYuvPath, NULL);
    const char * pcmPath = env->GetStringUTFChars(destPcmPath, NULL);
    mPlayerState->setDestYuvFilePath(yuvPath,pcmPath);
}







