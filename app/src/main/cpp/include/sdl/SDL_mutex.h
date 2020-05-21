//
// Created by Macro on 2020-02-25.
//

#ifndef JJPLAYER_SDL_MUTEX_H
#define JJPLAYER_SDL_MUTEX_H

#include <sys/types.h>
extern "C"{
#include <pthread.h>
};

#define SDL_MUTEX_TIMEDOUT 1
/** 互斥锁 */
class SDL_mutex {

public:

    pthread_mutex_t id;


public:
//    SDL_mutex *SDL_CreateMutex(void);
//
//    void SDL_DestroyMutex(SDL_mutex *mutex);
//
//    void SDL_DestroyMutexP(SDL_mutex **mutex);
//
//    int SDL_LockMutex(SDL_mutex *mutex);
//
//    int SDL_UnlockMutex(SDL_mutex *mutex);

        void createMutex();


};


#endif //JJPLAYER_SDL_MUTEX_H
