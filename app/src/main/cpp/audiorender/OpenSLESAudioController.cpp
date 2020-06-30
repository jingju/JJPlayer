//
// Created by Macro on 2020/6/19.
//

#include "audiorender/OpenSLESAudioController.h"

// this callback handler is called every time a buffer finishes playing
//todo 每次播放完缓冲的数据后，会回调该函数。
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {

    //todo  暂时注释掉里面所有的内容

//////    assert(bq == bqPlayerBufferQueue);
//////    assert(NULL == context);
//    // for streaming playback, replace this test by logic to find and fill the next buffer
//    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
//        SLresult result;
//        // enqueue another buffer
//        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
//        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
//        // which for this code example would indicate a programming error
//        if (SL_RESULT_SUCCESS != result) {
//            //todo 这个锁暂时没用到
////            pthread_mutex_unlock(&audioEngineLock);
//        }
//        (void)result;
//    } else {
//        //todo 释放掉重采样，暂时注释掉
////        releaseResampleBuf();
////        pthread_mutex_unlock(&audioEngineLock);
//    }
}

//初始化播放器
int OpenSLESAudioController::init(const AudioDeviceSpec *desired, AudioDeviceSpec *obtained) {

    SLresult result;
    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
        return -1;
    }

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
        return -1;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;

        return -1;
    }
    (void) result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID outputIds[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean outputReq[1] = {SL_BOOLEAN_FALSE};
//    todo id 和 req可以根据自己的需求定制
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, outputIds,
                                              outputReq);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;
    if (result != SL_RESULT_SUCCESS) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
        return -1;
    }
    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;
    if (result != SL_RESULT_SUCCESS) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
        return -1;
    }
    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    //todo 暂时注释掉
//    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
//                                              &outputMixEnvironmentalReverb);
//    if (SL_RESULT_SUCCESS == result) {
//        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
//                outputMixEnvironmentalReverb, &reverbSettings);
//        (void)result;
//    }



    //todo 创建播放器的过程
    /*
     * device native buffer size is another factor to minimize audio latency, not used in this
     * sample: we only play one giant buffer here
     */
//        bqPlayerBufSize = bufSize;
//    }


    /*
     * Enable Fast Audio when possible:  once we set the same rate to be the native, fast audio path
     * will be triggered
     */
//    if(bqPlayerSampleRate) {
//        format_pcm.samplesPerSec = bqPlayerSampleRate;       //sample rate in mili second
//    }
//    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    // 根据通道数设置通道mask
    SLuint32 channelMask;
    switch (desired->channels) {
        case 2: {
            channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            break;
        }
        case 1: {
            channelMask = SL_SPEAKER_FRONT_CENTER;
            break;
        }
        default: {
//            ("%s, invalid channel %d", __func__, desired->channels);
            return -1;
        }
    }

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                       OPENSLES_BUFFERS};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, getSLSampleRate(desired->freq),
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   channelMask, SL_BYTEORDER_LITTLEENDIAN};


    SLDataSource audioSrc = {&loc_bufq, &format_pcm};




    /*
     * create audio player:
     *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
     *     for fast audio case
     */
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    //todo 是否成功的判断
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the effect send interface
    bqPlayerEffectSend = NULL;
    if (0 == bqPlayerSampleRate) {
        result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                                 &bqPlayerEffectSend);
//        assert(SL_RESULT_SUCCESS == result);
        (void) result;
    }

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is
    // get the mute/solo interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
#endif

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
//    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // 这里计算缓冲区大小等参数
    bytes_per_frame = format_pcm.numChannels * format_pcm.bitsPerSample / 8;  // 一帧占多少字节
    milli_per_buffer = OPENSLES_BUFLEN;                                        // 每个缓冲区占多少毫秒
    frames_per_buffer = milli_per_buffer * format_pcm.samplesPerSec / 1000000;  // 一个缓冲区有多少帧数据
    bytes_per_buffer = bytes_per_frame * frames_per_buffer;                    // 一个缓冲区大小
    buffer_capacity = OPENSLES_BUFFERS * bytes_per_buffer;                    // 总的缓冲区容量

//    ALOGI("OpenSL-ES: bytes_per_frame  = %d bytes\n",  bytes_per_frame);
//    ALOGI("OpenSL-ES: milli_per_buffer = %d ms\n",     milli_per_buffer);
//    ALOGI("OpenSL-ES: frame_per_buffer = %d frames\n", frames_per_buffer);
//    ALOGI("OpenSL-ES: buffer_capacity  = %d bytes\n",  buffer_capacity);
//    ALOGI("OpenSL-ES: buffer_capacity  = %d bytes\n",  (int)buffer_capacity);

    if (obtained != NULL) {
        *obtained = *desired;
        obtained->size = (uint32_t) buffer_capacity;
        obtained->freq = format_pcm.samplesPerSec / 1000;
    }
    audioDeviceSpec = *desired;

    // 创建缓冲区
    buffer = (uint8_t *) malloc(buffer_capacity);
    if (!buffer) {
//        ALOGE("%s: failed to alloc buffer %d\n", __func__, (int)buffer_capacity);
        return -1;
    }

    // 填充缓冲区数据
    memset(buffer, 0, buffer_capacity);
    for (int i = 0; i < OPENSLES_BUFFERS; ++i) {
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
                                                 buffer + i * bytes_per_buffer,
                                                 bytes_per_buffer);
        if (result != SL_RESULT_SUCCESS) {
//            ALOGE("%s: slBufferQueueItf->Enqueue(000...) failed", __func__);
        }
    }

    return buffer_capacity;
}

void OpenSLESAudioController::createBufferQueueAudioPlayer(int sampleRate, int bufSize) {

}

void OpenSLESAudioController::releaseResampleBuf() {
    if (0 == bqPlayerSampleRate) {
        /*
         * we are not using fast path, so we were not creating buffers, nothing to do
         */
        return;
    }

//    free(resampleBuf);
//    resampleBuf = NULL;
}


/**
 * 开始播放
 *
 */

void OpenSLESAudioController::start() {
    uint8_t *next_buffer = NULL;
    int next_buffer_index = 0;
    //todo 暂时不开启线程。
    // 获取缓冲队列状态
    // 退出播放线程
//    if (abortRequest) {
//        break;
//    }
//
//    // 暂停
//    if (pauseRequest) {
//        continue;
//    }

    // 获取缓冲队列状态
    SLAndroidSimpleBufferQueueState slState = {0};
    SLresult slRet = (*bqPlayerBufferQueue)->GetState(bqPlayerBufferQueue, &slState);
    if (slRet != SL_RESULT_SUCCESS) {
//        ALOGE("%s: slBufferQueueItf->GetState() failed\n", __func__);
        mMutex.unlock();
    }
    // 判断暂停或者队列中缓冲区填满了
    mMutex.lock();
    //todo 这里暂时注掉一些状态的判断
    if (!abortRequest && (pauseRequest || slState.count >= OPENSLES_BUFFERS)) {
        while (slState.count >= OPENSLES_BUFFERS) {
            while (!abortRequest && (pauseRequest || slState.count >= OPENSLES_BUFFERS)) {

                if (!pauseRequest) {
                    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
                }

                //todo condition暂时不处理
//            mCondition.waitRelative(mMutex, 10 * 1000000);
                slRet = (*bqPlayerBufferQueue)->GetState(bqPlayerBufferQueue, &slState);
                if (slRet != SL_RESULT_SUCCESS) {
//                ALOGE("%s: slBufferQueueItf->GetState() failed\n", __func__);
                    mMutex.unlock();
                }

                if (pauseRequest) {
                    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
                }
            }

            if (!abortRequest && !pauseRequest) {
                (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
            }
//
        }
        //todo 暂时不管管，播放出来再说
        if (flushRequest) {
            (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
            flushRequest = 0;
        }
        mMutex.unlock();

        mMutex.lock();
        // 通过回调填充PCM数据
        //todo 这里的写法类似一个循环缓冲区
        if (audioDeviceSpec.callback != NULL) {
            next_buffer = buffer + next_buffer_index * bytes_per_buffer;
            next_buffer_index = (next_buffer_index + 1) % OPENSLES_BUFFERS;
            //todo
            audioDeviceSpec.callback(audioDeviceSpec.userdata, next_buffer, bytes_per_buffer);
        }
        mMutex.unlock();

        //todo 暂时不处理
        // 更新音量
//    if (updateVolume) {
//        if (slVolumeItf != NULL) {
//            SLmillibel level = getAmplificationLevel((leftVolume + rightVolume) / 2);
//            SLresult result = (*slVolumeItf)->SetVolumeLevel(slVolumeItf, level);
//            if (result != SL_RESULT_SUCCESS) {
//                ALOGE("slVolumeItf->SetVolumeLevel failed %d\n", (int) result);
//            }
//        }
//        updateVolume = false;
//    }
        // todo 暂时不处理
        // 刷新缓冲区还是将数据入队缓冲区
        if (flushRequest) {
            (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
            flushRequest = 0;
        } else {
            if (bqPlayerPlay != NULL) {
                (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
            }
            slRet = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, next_buffer,
                                                    bytes_per_buffer);
            if (slRet == SL_RESULT_SUCCESS) {
                // do nothing
            } else if (slRet == SL_RESULT_BUFFER_INSUFFICIENT) {
                // don't retry, just pass through
//            ALOGE("SL_RESULT_BUFFER_INSUFFICIENT\n");
            } else {
//            ALOGE("slBufferQueueItf->Enqueue() = %d\n", (int) slRet);
//            break;
            }
        }
    }

    if (bqPlayerPlay) {
        (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
    }

}

void OpenSLESAudioController::resume() {


}


void OpenSLESAudioController::pause() {


}

/**
 * 转换成SL的采样率
 * todo 注意这里的单位是毫赫兹
 * @param sampleRate
 * @return
 */
SLuint32 OpenSLESAudioController::getSLSampleRate(int sampleRate) {
    switch (sampleRate) {
        case 8000: {
            return SL_SAMPLINGRATE_8;
        }
        case 11025: {
            return SL_SAMPLINGRATE_11_025;
        }
        case 12000: {
            return SL_SAMPLINGRATE_12;
        }
        case 16000: {
            return SL_SAMPLINGRATE_16;
        }
        case 22050: {
            return SL_SAMPLINGRATE_22_05;
        }
        case 24000: {
            return SL_SAMPLINGRATE_24;
        }
        case 32000: {
            return SL_SAMPLINGRATE_32;
        }
        case 44100: {
            return SL_SAMPLINGRATE_44_1;
        }
        case 48000: {
            return SL_SAMPLINGRATE_48;
        }
        case 64000: {
            return SL_SAMPLINGRATE_64;
        }
        case 88200: {
            return SL_SAMPLINGRATE_88_2;
        }
        case 96000: {
            return SL_SAMPLINGRATE_96;
        }
        case 192000: {
            return SL_SAMPLINGRATE_192;
        }
        default: {
            return SL_SAMPLINGRATE_44_1;
        }
    }
}
