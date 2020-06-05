//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_AUDIOOUTPUT_H
#define JJPLAYER_AUDIOOUTPUT_H


#include <sys/types.h>
#include <pthread.h>

class AudioOutput {
private:
    pthread_t audioOutputThread;
    void* audioRender;
    int fillAudioDataCallback();

public:
    bool start(int channel,int sampleRate,int format);
    void stop();
    void run();
};


#endif //JJPLAYER_AUDIOOUTPUT_H
