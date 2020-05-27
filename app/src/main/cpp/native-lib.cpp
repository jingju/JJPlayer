#include <jni.h>
#include <string>
#include <player/JJMediaPlayer.h>

JJMediaPlayer *player;

extern "C" JNIEXPORT jstring JNICALL
Java_com_jingju_jjplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_jingju_jjplayer_JJMediaPlayer__1setUp(JNIEnv *env, jobject thiz,
                                               jobject weak_this) {
    // TODO: implement _setUp()
    //创建播放器对象
    player =new JJMediaPlayer();
    //播放器保持一个本地mediaplayer的类
    player->setJavaJJPlayerRef(weak_this);
    //todo 其它一些设置

}



/**
 *
 * 出事化视频显示模块（opengl相关的本地初始化工作）
 *
 */

extern "C"
JNIEXPORT void JNICALL
Java_com_jingju_jjplayer_JJMediaPlayer__1setVideoSurface(JNIEnv *env, jobject thiz,
                                                         jobject surface) {
    // TODO: implement _setVideoSurface()
    if (surface != 0 ) {
        if(NULL!=player){
            player->setNativieSurface(env,surface);
        }
    }
}

/**
 * 设置视频路径
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_jingju_jjplayer_JJMediaPlayer__1setDataSource(JNIEnv *env, jobject thiz, jstring path,
                                                       jobjectArray keys, jobjectArray values) {
    if(NULL!=player){
        player->setDataSource(env,path, keys, values);
    }
}