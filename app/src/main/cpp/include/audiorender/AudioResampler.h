//
// Created by Macro on 2020/6/28.
//

#ifndef JJPLAYER_AUDIORESAMPLER_H
#define JJPLAYER_AUDIORESAMPLER_H

#include "BaseAudioController.h"

/**
 * 音频参数
 */
typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

class AudioResampler {
public:
//    AudioResampler(PlayerState *playerState, AudioDecoder *audioDecoder, MediaSync *mediaSync);
    AudioResampler();

    virtual ~AudioResampler();

    int setResampleParams(AudioDeviceSpec *spec, int64_t wanted_channel_layout);

    void pcmQueueCallback(uint8_t *stream, int len);

private:
    int audioSynchronize(int nbSamples);

    int audioFrameResample();


};


#endif //JJPLAYER_AUDIORESAMPLER_H
