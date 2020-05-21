//
// Created by Macro on 2020-02-26.
//

#ifndef JJPLAYER_SDL_THREAD_H
#define JJPLAYER_SDL_THREAD_H


#include <sys/types.h>
#include "CommonTools.h"

#define LOG_TAG "SDL_Thread"
typedef enum {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH
} SDL_ThreadPriority;

class SDL_Thread {
public:
    SDL_Thread(int (*fn)(void *), void *data, const char *name);
    pthread_t id;
    int (*func)(void *);
    void *data;
    char name[32];
    int retval;
public:
    int  SDL_Create();
    int         SDL_SetThreadPriority(SDL_ThreadPriority priority);
    void        SDL_WaitThread(SDL_Thread *thread, int *status);
    void        SDL_DetachThread(SDL_Thread *thread);
    static void *SDL_RunThread(void *data);
};


#endif //JJPLAYER_SDL_THREAD_H
