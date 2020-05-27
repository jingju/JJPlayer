//
// Created by Macro on 2020/4/27.
//

#include "player/JJMediaPlayer.h"

void JJMediaPlayer::prepareAsync() {
        mutex.lock();
        prepareAsyncl();
        mutex.unlock();
}


void JJMediaPlayer::prepareAsyncl() {
        //todo  消息队列初始化，并放入第一个消息
        jjplayer->changState(MP_STATE_ASYNC_PREPARING);
        //todo 开启消息循环的线程
        std::thread threadMessageLoop(message_loop);
        jjplayer->repareAsyncl();

}

/**
 * todo 不断获取消息，进行下一步的处理。
 */
void JJMediaPlayer::message_loop() {

}


void JJMediaPlayer::setJavaJJPlayerRef(void *ref){
        weak_this= ref;
}


void JJMediaPlayer::setNativieSurface(JNIEnv *env, jobject surface) {
        if(NULL!=jjplayer){
                jjplayer->setNativeSurface(env,surface);
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
    sourcePath=env->GetStringUTFChars(path,NULL);
}

