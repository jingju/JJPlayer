//
// Created by Macro on 2020/6/28.
//

extern "C"{
#include <libavutil/time.h>
}
#include "../include/audiorender/AudioResampler.h"

AudioResampler::AudioResampler(PlayerState *state) {
    playerState = state;
    audioState = (AudioState *) av_mallocz(sizeof(AudioState));
    memset(audioState, 0, sizeof(AudioState));
}

AudioResampler::~AudioResampler() {

}

int AudioResampler::setResampleParams(AudioDeviceSpec *spec, int64_t wanted_channel_layout) {

    audioState->audioParamsSrc = audioState->audioParamsTarget;
    audioState->audio_hw_buf_size = spec->size;
    audioState->bufferSize = 0;
    audioState->bufferIndex = 0;
    audioState->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
    audioState->audio_diff_avg_count = 0;
    audioState->audio_diff_threshold =
            (double) (audioState->audio_hw_buf_size) / audioState->audioParamsTarget.bytes_per_sec;

    audioState->audioParamsTarget.fmt = AV_SAMPLE_FMT_S16;
    audioState->audioParamsTarget.freq = spec->freq;
    audioState->audioParamsTarget.channel_layout = wanted_channel_layout;
    audioState->audioParamsTarget.channels = spec->channels;
    audioState->audioParamsTarget.frame_size = av_samples_get_buffer_size(NULL,
                                                                          audioState->audioParamsTarget.channels,
                                                                          1,
                                                                          audioState->audioParamsTarget.fmt,
                                                                          1);
    audioState->audioParamsTarget.bytes_per_sec = av_samples_get_buffer_size(NULL,
                                                                             audioState->audioParamsTarget.channels,
                                                                             audioState->audioParamsTarget.freq,
                                                                             audioState->audioParamsTarget.fmt,
                                                                             1);

    if (audioState->audioParamsTarget.bytes_per_sec <= 0 ||
        audioState->audioParamsTarget.frame_size <= 0) {
        av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
        return -1;
    }
    return 0;
}

/**
 * AVFrame 的重新采样
 * @param frame
 * @return
 */
int AudioResampler::resample(AVFrame *frame) {
    //todo 这里不初始化的话，会随便付给内存
    int data_size, resampled_data_size=0;
    int64_t dec_channel_layout=0;
    int wanted_nb_samples=0;
    int translate_time = 1;
    int ret = -1;

    // todo 处于暂停状态
//    if (!audioDecoder || playerState->abortRequest || playerState->pauseRequest) {
//        return -1;
//    }


    data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(frame),
                                           frame->nb_samples,
                                           (AVSampleFormat) frame->format, 1);

    dec_channel_layout =
            (frame->channel_layout && av_frame_get_channels(frame) ==
                                      av_get_channel_layout_nb_channels(frame->channel_layout))
            ? frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(frame));
    //todo 待定
    wanted_nb_samples = audioSynchronize(frame->nb_samples);

    // 帧格式跟源格式不对？？？？
    if (frame->format != audioState->audioParamsSrc.fmt
        || dec_channel_layout != audioState->audioParamsSrc.channel_layout
        || frame->sample_rate != audioState->audioParamsSrc.freq
        || (wanted_nb_samples != frame->nb_samples && !audioState->swr_ctx)) {

        swr_free(&audioState->swr_ctx);
        //重新创建一个swr_ctx
        audioState->swr_ctx = swr_alloc_set_opts(NULL, audioState->audioParamsTarget.channel_layout,
                                                 audioState->audioParamsTarget.fmt,
                                                 audioState->audioParamsTarget.freq,
                                                 dec_channel_layout, (AVSampleFormat) frame->format,
                                                 frame->sample_rate, 0, NULL);
        //初始化swr_ctx
        if (!audioState->swr_ctx || swr_init(audioState->swr_ctx) < 0) {
            av_log(NULL, AV_LOG_ERROR,
                   "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
                   frame->sample_rate,
                   av_get_sample_fmt_name((AVSampleFormat) frame->format),
                   av_frame_get_channels(frame),
                   audioState->audioParamsTarget.freq,
                   av_get_sample_fmt_name(audioState->audioParamsTarget.fmt),
                   audioState->audioParamsTarget.channels);
            swr_free(&audioState->swr_ctx);
            return -1;
        }
        //源数据参数
        audioState->audioParamsSrc.channel_layout = dec_channel_layout;
        audioState->audioParamsSrc.channels = av_frame_get_channels(frame);
        audioState->audioParamsSrc.freq = frame->sample_rate;
        audioState->audioParamsSrc.fmt = (AVSampleFormat) frame->format;
    }

    // 音频重采样处理
    if (audioState->swr_ctx) {
        const uint8_t **in = (const uint8_t **) frame->extended_data;
        uint8_t **out = &audioState->resampleBuffer;//输出的数据缓存
        //输出的采样的数量
        int out_count = (int64_t) wanted_nb_samples * audioState->audioParamsTarget.freq /
                        frame->sample_rate + 256;
        //输出缓冲区大小
        int out_size = av_samples_get_buffer_size(NULL, audioState->audioParamsTarget.channels,
                                                  out_count, audioState->audioParamsTarget.fmt, 0);
        int len2;
        if (out_size < 0) {
            av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
            return -1;
        }
        //如果期望的样本数不等于实际的样本数，需要进行重采样的补偿计算
        if (wanted_nb_samples != frame->nb_samples) {
            //计算重采样的补偿
            if (swr_set_compensation(audioState->swr_ctx, (wanted_nb_samples - frame->nb_samples) *
                                                          audioState->audioParamsTarget.freq /
                                                          frame->sample_rate,
                                     wanted_nb_samples * audioState->audioParamsTarget.freq /
                                     frame->sample_rate) < 0) {
                av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
                return -1;
            }
        }
        //在resampleBuffer上重新申请分配resampleSize后面out_size大小的内存
        av_fast_malloc(&audioState->resampleBuffer, &audioState->resampleSize, out_size);
        if (!audioState->resampleBuffer) {
            return AVERROR(ENOMEM);
        }
        len2 = swr_convert(audioState->swr_ctx, out, out_count, in, frame->nb_samples);
        if (len2 < 0) {
            av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
            return -1;
        }
        if (len2 == out_count) {
            av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
            if (swr_init(audioState->swr_ctx) < 0) {
                swr_free(&audioState->swr_ctx);
            }
        }
        audioState->outputBuffer = audioState->resampleBuffer;
        resampled_data_size = len2 * audioState->audioParamsTarget.channels *
                              av_get_bytes_per_sample(audioState->audioParamsTarget.fmt);

        // 变速变调处理
        //todo 暂时不做变调处理
//        if ((playerState->playbackRate != 1.0f || playerState->playbackPitch != 1.0f) &&
//            !playerState->abortRequest) {
//            int bytes_per_sample = av_get_bytes_per_sample(audioState->audioParamsTarget.fmt);
//            av_fast_malloc(&audioState->soundTouchBuffer, &audioState->soundTouchBufferSize,
//                           out_size * translate_time);
//            for (int i = 0; i < (resampled_data_size / 2); i++) {
//                audioState->soundTouchBuffer[i] = (audioState->resampleBuffer[i * 2] |
//                                                   (audioState->resampleBuffer[i * 2 + 1] << 8));
//            }
//            if (!soundTouchWrapper) {
//                soundTouchWrapper = new SoundTouchWrapper();
//            }
//            int ret_len = soundTouchWrapper->translate(audioState->soundTouchBuffer,
//                                                       (float) (playerState->playbackRate),
//                                                       (float) (playerState->playbackPitch != 1.0f
//                                                                ? playerState->playbackPitch :
//                                                                1.0f / playerState->playbackRate),
//                                                       resampled_data_size / 2, bytes_per_sample,
//                                                       audioState->audioParamsTarget.channels,
//                                                       frame->sample_rate);
//            if (ret_len > 0) {
//                audioState->outputBuffer = (uint8_t *) audioState->soundTouchBuffer;
//                resampled_data_size = ret_len;
//            } else {
//                translate_time++;
//                av_frame_unref(frame);
//                return -1;
//            }
//        }
//    } else {
        audioState->outputBuffer = frame->data[0];
        resampled_data_size = data_size;
//    }

        // 利用pts更新音频时钟
        if (frame->pts != AV_NOPTS_VALUE) {
            audioState->
                    audioClock = frame->pts * av_q2d((AVRational) {1, frame->sample_rate})
                                 + (double) frame->nb_samples / frame->sample_rate;
        } else {
            audioState->
                    audioClock = NAN;
        }

        // 使用完成释放引用，防止内存泄漏
        av_frame_unref(frame);
        return resampled_data_size;
    }
}

/**
 * todo 暂时先不处理
 * @param nbSamples
 * @return
 */
int AudioResampler::audioSynchronize(int nbSamples) {

    int wanted_nb_samples = nbSamples;

    // 如果时钟不是同步到音频流，则需要进行对音频频进行同步处理
    //todo 暂时都以音频为同步主时钟
//    if (playerState->syncType != AV_SYNC_AUDIO) {
//        double diff, avg_diff;
//        int min_nb_samples, max_nb_samples;
//        diff = mediaSync ? mediaSync->getAudioDiffClock() : 0;
//        if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
//            audioState->audio_diff_cum = diff + audioState->audio_diff_avg_coef * audioState->audio_diff_cum;
//            if (audioState->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
//                audioState->audio_diff_avg_count++;
//            } else {
//                avg_diff = audioState->audio_diff_cum * (1.0 - audioState->audio_diff_avg_coef);
//
//                if (fabs(avg_diff) >= audioState->audio_diff_threshold) {
//                    wanted_nb_samples = nbSamples + (int)(diff * audioState->audioParamsSrc.freq);
//                    min_nb_samples = ((nbSamples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
//                    max_nb_samples = ((nbSamples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
//                    wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
//                }
//            }
//        } else {
//            audioState->audio_diff_avg_count = 0;
//            audioState->audio_diff_cum = 0;
//        }
//    }

    return wanted_nb_samples;
}

void AudioResampler::pcmQueueCallback(uint8_t *stream, int len) {

    int bufferSize, length;

    // todo 没有音频解码器时，直接返回
//    if (!audioDecoder) {
//        memset(stream, 0, len);
//        return;
//    }

    audioState->audio_callback_time = av_gettime_relative();
    while (len > 0) {
        if (audioState->bufferIndex >= audioState->bufferSize) {
//            bufferSize = audioFrameResample();
//            AVFrame *frame =playerState->mAudioDecoder->frameQueue->wait_and_pop();
            Frame *frame =playerState->mAudioDecoder->mDecodeFrameQueue2->wait_and_pop();
            bufferSize = resample(frame->frame);
//            bufferSize = resample(frame);
            if (bufferSize < 0) {
                audioState->outputBuffer = NULL;
                audioState->bufferSize = (unsigned int) (AUDIO_MIN_BUFFER_SIZE / audioState->audioParamsTarget.frame_size
                                                         * audioState->audioParamsTarget.frame_size);
            } else {
                audioState->bufferSize = bufferSize;
            }
            audioState->bufferIndex = 0;
        }

        length = audioState->bufferSize - audioState->bufferIndex;
        if (length > len) {
            length = len;
        }
        // 复制经过转码输出的PCM数据到缓冲区中

//        if (audioState->outputBuffer != NULL && !playerState->mute) {//不等于NULL或者静音
        if (audioState->outputBuffer != NULL) {
            memcpy(stream, audioState->outputBuffer + audioState->bufferIndex, length);
        } else {
            memset(stream, 0, length);
        }
        len -= length;
        stream += length;
        audioState->bufferIndex += length;
    }
    audioState->writeBufferSize = audioState->bufferSize - audioState->bufferIndex;
//    /* Let's assume the audio driver that is used by SDL has two periods. */
//    //todo 音频时间的同步,这里有待验证
//    upDataAudioClock();


}

//todo 更新音频时钟
void AudioResampler::upDataAudioClock() {
    if (!isnan(audioState->audioClock)) {
        playerState->setClockAt(&playerState->mAudioClcok,
                                audioState->audioClock - (double)(2*audioState->audio_hw_buf_size+audioState->writeBufferSize)/audioState->audioParamsTarget.bytes_per_sec
                ,playerState->mAudioClcok.serial,audioState->audio_callback_time/1000000.0 );
        playerState->syncClockToSlave(&playerState->mExctClock,&playerState->mAudioClcok);
    }

}