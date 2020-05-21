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





}