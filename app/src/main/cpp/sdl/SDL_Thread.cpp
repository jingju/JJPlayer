//
// Created by Macro on 2020-02-26.
//

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "sdl/SDL_Thread.h"
//todo
//#ifdef __ANDROID__
//#include "ijksdl_android_jni.h"
SDL_Thread::SDL_Thread( int (*fn)(void *), void *data, const char *name) {
    func = fn;
    this->data = data;
    strlcpy(this->name, name, sizeof(name) - 1);
}

/**
 *
 * @return 0 代表创建成功,非0代表失败
 */
int SDL_Thread::SDL_Create() {
    int retval = pthread_create(&id, NULL, SDL_RunThread, this);
    return retval;
}

void* SDL_Thread::SDL_RunThread(void *data) {
    SDL_Thread *thread = static_cast<SDL_Thread *>(data);
    LOGI(LOG_TAG,"SDL_RunThread: [%d] %s\n", (int)gettid(),thread->name);
    pthread_setname_np(pthread_self(), thread->name);
    thread->retval = thread->func(data);
#ifdef __ANDROID__
    //todo 这里要dettache 掉jni层的线程
//    SDL_JNI_DetachThreadEnv();
#endif
    return NULL;

}
