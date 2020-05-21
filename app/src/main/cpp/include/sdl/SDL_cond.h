//
// Created by Macro on 2020-02-25.
//

#ifndef JJPLAYER_SDL_COND_H
#define JJPLAYER_SDL_COND_H


#include <sys/types.h>
#include "SDL_mutex.h"

class SDL_cond {

public:
    pthread_cond_t id;
public:
//    SDL_cond *SDL_CreateCond(void);

    void SDL_DestroyCond(SDL_cond *cond);

    void SDL_DestroyCondP(SDL_cond **mutex);

    int SDL_CondSignal(SDL_cond *cond);

    int SDL_CondBroadcast(SDL_cond *cond);

    int SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, uint32_t ms);

    int SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex);
};


#endif //JJPLAYER_SDL_COND_H
