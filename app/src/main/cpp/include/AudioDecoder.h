//
// Created by Macro on 2020/10/20.
//

#ifndef JJPLAYER_AUDIODECODER_H
#define JJPLAYER_AUDIODECODER_H


#include "Decoder.h"

class AudioDecoder : public Decoder{


public:
    bool  isPending=false;
public:
    int decodeAudioPacket(AVPacket *pkt,AVFrame *frame);

    int audioThread();

    void start();

    void stop();
};


#endif //JJPLAYER_AUDIODECODER_H
