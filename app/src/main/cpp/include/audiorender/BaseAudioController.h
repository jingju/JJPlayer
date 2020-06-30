//
// Created by Macro on 2020/6/23.
//

#ifndef JJPLAYER_BASEAUDIOCONTROLLER_H
#define JJPLAYER_BASEAUDIOCONTROLLER_H

extern "C"{
#include <libavutil/samplefmt.h>

};
// 音频PCM填充回调
typedef void (*AudioPCMCallback) (void *userdata, uint8_t *stream, int len);

typedef struct AudioDeviceSpec {
    int freq;                   // 采样率
    AVSampleFormat format;      // 音频采样格式
    uint8_t channels;           // 声道
    uint16_t samples;           // 采样大小
    uint32_t size;              // 缓冲区大小
    AudioPCMCallback callback;  // 音频回调
    void *userdata;             // 音频上下文
} AudioDeviceSpec;

class BaseAudioController {

public:
//    BaseAudioController();
//    virtual ~BaseAudioController();

    virtual int init(const AudioDeviceSpec *desired, AudioDeviceSpec *obtained)=0;

    virtual void start()=0;

    virtual void resume()=0;
    virtual void pause()=0;


};


#endif //JJPLAYER_BASEAUDIOCONTROLLER_H
