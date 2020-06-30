//
// Created by Macro on 2020/4/27.
//

#include <player/JJPlayer.h>
#include <player/JJMediaPlayer.h>

void JJPlayer::init() {

}


void JJPlayer::setSourcePath(const char *sourcePath) {
    mSourPath=sourcePath;
}


/**
 * 创建window
 * @param env
 * @param surface
 */
void JJPlayer::setNativeSurface(JNIEnv *env, jobject surface) {
        _window = ANativeWindow_fromSurface(env, surface);
}

void JJPlayer::prepareAsync() {

}

void JJPlayer::changState(int stateCode) {
    AVMessage *message = new AVMessage;
    message->what = stateCode;
    mMessageQueue.push(message);//准备阶段的message
}


AVMessage *JJPlayer::getMessage() {
    return mMessageQueue.wait_and_pop();
}


/**
 * todo 准本播放
 */
int JJPlayer::repareAsyncl() {
    if (!videoOutput) {
        return -1;
    }

    streamOpen();

    /**
     * todo 后面的一些操作待定
     */

    return 0;
}

/**
 * 解码前一些相关线程的初始化工作
 * @param fileName
 */
void JJPlayer::streamOpen() {
    /**
     * todo 初始化 VideoState 类，记录player的一些状态
     */






    //=======================
    //todo 一些播放器状态的判断



    //todo 初始化video audio 和 ext 实际时间的时钟



    //todo 相关声音的初始化，线程条件变量的初始化，精确的seek



    //===================================
    //todo 初始化视频刷新线程
    mVideoRefreshThread = std::thread(&JJPlayer::videoRefreshThread, this);
    //todo 初始化解码器
    initDecoder();
    //todo 流数据的读取，解码
    mReadThread = std::thread(&JJPlayer::readThread, this);
}

/**
 * todo 不断的从视频队列取消息，刷新到openGL上
 */
int JJPlayer::videoRefreshThread() {
    //todo 不断读取视频，并渲染播放


    return 0;
}


/**
 * 读取线程的初始化
 * @return
 */

int JJPlayer::readThread() {
    //todo 一些初始化工作

    /**
     * todo 等待的互斥量的初始化mutex
     *      最近的video_stream的初始化
     *      最近的audio_stream的初始化。
     *      最近的字幕流的初始化
     *
     *      AVFormateContext的申请
     */
    //todo 初始化egl相关

    initVideoRender();
    createAudioRender();
    mDecoder->openFile(mSourPath,mRenderController,mAudioController);



//==========================
    //todo 从packet queue 中读取数据，解码后，存到对应的frame queue以供后续使用
//    if(st_index[AVMEDIA_TYPE_VIDEO])
//        stream_componet_open()// todo 打开流，从packet queue中读取数据解码，放到对应的frame queue

//    if(st_index[AVMEDIA_TYPE_AUDIO])


//     if(st_index[AVMEDIA_TYPE_SUBTITLE])

//===================

    //todo 循环将读取到的packet 缓存到packet queue


    return 0;
}

/**
 * todo 开启对应的解码线程
 * @return
 */
int JJPlayer::streamComponentOpen() {

    //todo 打开音频渲染通道
//        audio_open


//    //todo 初始化音频解码器，解码
//    std::thread mAudioThread(audioThread);
//
//
//
//
//    //todo 初始化视频解码器，解码
//    std::thread mVideoThread(videoThread);
//
//
//
//    //todo 初始化字幕解码器，解码
//
//    std::thread mSubtitleThread(subtitleThread);

    return 0;
}

/**
 * todo 音频解码线程
 * @return
 */
int JJPlayer::audioThread() {
    //todo 解码，解码成功后，

    // 将AVFrame 放到队列里面
    return 0;
}

/**
 * todo 视频解码线程
 * @return
 */
int JJPlayer::videoThread() {

    return 0;
}

/**
 * todo
 * @return
 */
int JJPlayer::subtitleThread() {

    return 0;
}


void JJPlayer::initDecoder() {
        mDecoder=new Decoder;
}

/**
 * 初始化视频的渲染类
 */
void JJPlayer::initVideoRender() {
    mRenderController=new VideoRenderController;
    mRenderController->initEGL(_window);
    mRenderController->prepareRender();

}

void JJPlayer::createAudioRender() {
    mAudioController= new OpenSLESAudioController;
}



JJPlayer::~JJPlayer() {
    delete mDecoder;

}