//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_VIDEOOUTPUT_H
#define JJPLAYER_VIDEOOUTPUT_H


#include <sys/types.h>
#include <pthread.h>

class VideoOutput {
private:
    pthread_t videoOutputThread;
    void* videoRender;
    int fillVideoFrameCallBack();

public:

    bool start(int format,int width,int height);
    void run();
    void stop();

};


#endif //JJPLAYER_VIDEOOUTPUT_H
