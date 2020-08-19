//
// Created by Macro on 2020/6/28.
//

#ifndef JJPLAYER_AUDIORESAMPLER_H
#define JJPLAYER_AUDIORESAMPLER_H
extern "C"{
#include <libavutil/frame.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
};

#include <soundtouch/SoundTouchWrapper.h>
#include <player/PlayerState.h>
#include "BaseAudioController.h"

/**
 * 音频参数
 */
typedef struct AudioParams {
    int freq;//采样率
    int channels;//通道数量
    int64_t channel_layout;//通道类型
    enum AVSampleFormat fmt;//音频格式
    int frame_size;//一帧数据的大小
    int bytes_per_sec;//每秒几个字节
} AudioParams;

/**
 * 音频重采样状态结构体
 */
typedef struct AudioState {
    double audioClock;                      // 音频时钟
    double audio_diff_cum;
    double audio_diff_avg_coef;
    double audio_diff_threshold;
    int audio_diff_avg_count;
    int audio_hw_buf_size;
    uint8_t *outputBuffer;                  // 输出缓冲大小
    uint8_t *resampleBuffer;                // 重采样大小
    short *soundTouchBuffer;                // SoundTouch缓冲
    unsigned int bufferSize;                // 缓冲大小
    unsigned int resampleSize;              // 重采样大小
    unsigned int soundTouchBufferSize;      // SoundTouch处理后的缓冲大小大小
    int bufferIndex;
    int writeBufferSize;                    // 写入大小
    SwrContext *swr_ctx;                    // 音频转码上下文
    int64_t audio_callback_time;            // 音频回调时间
    AudioParams audioParamsSrc;             // 音频原始参数
    AudioParams audioParamsTarget;          // 音频目标参数
} AudioState;





class AudioResampler {
public:
//    AudioResampler(PlayerState *playerState, AudioDecoder *audioDecoder, MediaSync *mediaSync);
    AudioResampler(PlayerState *state);

    virtual ~AudioResampler();

    int setResampleParams(AudioDeviceSpec *spec, int64_t wanted_channel_layout);

    void pcmQueueCallback(uint8_t *stream, int len);

private:
    int audioSynchronize(int nbSamples);

    int resample(AVFrame *frame);
private:     // 音频解码器
    AudioState *audioState;                 // 音频重采样状态
    PlayerState *playerState;
    SoundTouchWrapper *soundTouchWrapper;   // 变速变调处理

};


#endif //JJPLAYER_AUDIORESAMPLER_H
