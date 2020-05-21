//
// Created by Macro on 2020-02-25.
//

#include "sdl/SDL_mutex.h"

//int SDL_mutex::SDL_LockMutex(SDL_mutex *mutex) {
//
//    if(mutex!=NULL){
//        return -1;
//    }
//    return pthread_mutex_lock(&mutex->id);
//}
//
//
//
//int SDL_mutex::SDL_UnlockMutex(SDL_mutex *mutex) {
//
//    if(mutex==NULL){
//        return -1;
//    }
//    return pthread_mutex_unlock(&mutex->id);
//}
//
//
//void SDL_mutex::SDL_DestroyMutex(SDL_mutex *mutex) {
//    if(mutex != NULL){
//        pthread_mutex_destroy(&mutex->id);
//    }
//}
//
//
//
//void SDL_mutex::SDL_DestroyMutexP(SDL_mutex **mutex) {
//    if (mutex!=NULL) {
//        SDL_DestroyMutex(*mutex);
//        *mutex = NULL;
//    }
//}

void SDL_mutex::createMutex() {

    pthread_mutex_init(&id,NULL);
}