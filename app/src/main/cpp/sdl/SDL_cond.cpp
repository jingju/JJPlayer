//
// Created by Macro on 2020-02-25.
//

#include <asm/errno.h>
#include "sdl/SDL_cond.h"

void SDL_cond::SDL_DestroyCond(SDL_cond *cond) {

    if (cond != NULL) {
        pthread_cond_destroy(&cond->id);
    }
}

void SDL_cond::SDL_DestroyCondP(SDL_cond **cond) {

    if (cond != NULL) {
        SDL_DestroyCond(*cond);
        *cond = NULL;
    }
}

int SDL_cond::SDL_CondSignal(SDL_cond *cond) {

    if (cond == NULL) {
        return -1;
    }
    return pthread_cond_signal(&cond->id);
}

int SDL_cond::SDL_CondBroadcast(SDL_cond *cond) {

    if (cond == NULL) {
        return -1;
    }

    return pthread_cond_broadcast(&cond->id);
}

//TODO 给条件加超时时间
int SDL_cond::SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, uint32_t ms) {

    int retval;
    struct timeval delta;//当前时间
    struct timespec abstime;

    if (cond == NULL || mutex == NULL) {
        return -1;
    }

    gettimeofday(&delta, NULL);

    abstime.tv_sec = delta.tv_sec + (ms / 1000);
    abstime.tv_nsec = (delta.tv_usec + (ms % 1000) * 1000) * 1000;
    if (abstime.tv_nsec > 1000000000) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000;
    }

    while (1) {
        retval = pthread_cond_timedwait(&cond->id, &mutex->id, &abstime);
        if (retval == 0)
            return 0;
        else if (retval == EINTR)
            continue;
        else if (retval == ETIMEDOUT)
            return SDL_MUTEX_TIMEDOUT;
        else
            break;
    }

    return -1;

}

int SDL_cond::SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex) {

    if (cond == NULL || mutex == NULL)
        return -1;
    return pthread_cond_wait(&cond->id, &mutex->id);
}


