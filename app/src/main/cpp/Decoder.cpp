//
// Created by Macro on 2020-02-14.
//
#include "common/CommonTools.h"

#define LOG_TAG "VideoDecoder"

#include "Decoder.h"


Decoder::Decoder() {
}

/**打开资源 */
int Decoder::openFile(const char *src_filename,VideoRenderController *controler,OpenSLESAudioController *audioController ) {
    mRenderController=controler;
    mAudioController=audioController;

    int ret = 0;
    //todo 除了注册，还要请求文件的读写权限
    av_register_all();

    fmt_ctx = avformat_alloc_context();

    //todo 暂时先不设置中中断的回调。
//    AVIOInterruptCB int_cb = {&interrupt_cb, this};
//    fmt_ctx->interrupt_callback = int_cb;

    /** open input file, and allocate format context */
    if ((ret = avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
        LOGI("Could not open source file %s\n", src_filename);
        return -1;
    }

    /** retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        LOGI("Could not find stream information\n");
        return -1;

    }

    //todo 现在在readthread里面  打开 videothread 和 audiothread
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        enum AVMediaType mediaType = fmt_ctx->streams[i]->codecpar->codec_type;
        if (mediaType == AVMEDIA_TYPE_VIDEO) {
            LOGI("find video stream");
            video_stream_idx = i;
            video_stream = fmt_ctx->streams[i];
            //todo==========metata信息打印=============
//            AVDictionary * dic=video_stream->metadata;
//            AVDictionaryEntry * entry= nullptr;
//            while ((entry = av_dict_get(dic, "", entry, AV_DICT_IGNORE_SUFFIX))) {
//                fprintf(stdout, "key: %s, value: %s\n", entry->key, entry->value);
//            }
            //todo==========metata 信息打印=============
            open_codec_context(video_dec_ctx, video_stream, AVMEDIA_TYPE_VIDEO);
        } else if (mediaType == AVMEDIA_TYPE_AUDIO) {
            LOGI("finfd aduio stream");
            audio_stream_idx = i;
            audio_stream = fmt_ctx->streams[i];
            open_codec_context(audio_dec_ctx, audio_stream, AVMEDIA_TYPE_AUDIO);
        }
    }
    //todo 开始读取数据放到对应的队列里面。
    decode();

    return ret;
}

/** connect time out call back*/
//int Decoder::interrupt_cb(void *ctx) {
//    //todo 解码失败的重试
//
//}


int Decoder::open_codec_context(AVCodecContext *dec_ctx, AVStream *st, AVMediaType type) {

    //todo 在这里开启解video 和 audio 的解码线程


    AVDictionary *opts = NULL;
    AVCodec *dec = NULL;

    /* find mDecoder for the stream */
    dec = avcodec_find_decoder(st->codecpar->codec_id);

    if (!dec) {
        LOGI(LOG_TAG, "Failed to find %s codec\n",
             av_get_media_type_string(type));
        return -1;
    }

    /* Allocate a codec context for the mDecoder */
    dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx) {
        LOGI("Failed to allocate the %s codec context\n",
             av_get_media_type_string(type));
        return -1;
    }

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(dec_ctx, st->codecpar)) < 0) {
        LOGI("Failed to copy %s codec parameters to mDecoder context\n",
             av_get_media_type_string(type));
        return ret;
    }

    //todo 最后看看有用没用
    if (type == AVMEDIA_TYPE_VIDEO) {
        /* allocate image where the decoded image will be put */
        width = dec_ctx->width;
        height = dec_ctx->height;
        pix_fmt = dec_ctx->pix_fmt;

        ret = av_image_alloc(video_dst_data, video_dst_linesize, width, height, pix_fmt, 1);

        if (ret < 0) {
            release();
        }

        video_dst_bufsize = ret;
    }
    /* Init the decoders, with or without reference counting */
    av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
    if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
        LOGI("Failed to open %s codec\n",
             av_get_media_type_string(type));
        return ret;
    }

    //todo 在这里开启解video 和 audio 的解码线程
    if (type == AVMEDIA_TYPE_VIDEO) {
        //todo 初始化video 输出相关

        mVideoThead = std::thread(&Decoder::videoThread, this);
    } else if (type == AVMEDIA_TYPE_AUDIO) {

        /**
         * todo 解码出来的采样率和通道数量
         *      我门所希望的采样率和通道数
         */
//        audio_dec_ctx->


        //todo 初始化audio 输出相关
        openAudioController(dec_ctx->channel_layout,dec_ctx->channels,dec_ctx->sample_rate);
        //todo 初始化重采样器
//        if (!audioResampler) {
//            //todo 暂时不处理播放状态和音频重采样
//            audioResampler = new AudioResampler();
//        }
//         // 设置需要重采样的参数
//         audioResampler->setResampleParams(&spec, wanted_channel_layout);
        mAudioThread = std::thread(&Decoder::audioThread, this);
    }
//    //todo 解码，放到解码后的队列
//    decode();
//
//    int ret,stream_index;
//    AVStream *st;
//    AVCodec *dec=NULL;
//    AVDictionary *opts=NULL;
//
//    ret = av_find_best_stream(fmt_ctx,type,-1,-1,NULL,NULL);
//
//    if (ret < 0) {
//        LOGI("Could not find %s stream in input file '%s'\n",
//                av_get_media_type_string(type),"");
//        return ret;
//    } else {
//        stream_index = ret;
//        st=fmt_ctx->streams[stream_index];
//
//        /* find mDecoder for the stream */
//        dec = avcodec_find_decoder(st->codecpar->codec_id);
//
//        if(!dec){
//                LOGI(LOG_TAG, "Failed to find %s codec\n",
//                        av_get_media_type_string(type));
//                return -1;
//        }
//
//        /* Allocate a codec context for the mDecoder */
//        *dec_ctx = avcodec_alloc_context3(dec);
//        if (!*dec_ctx) {
//            LOGI("Failed to allocate the %s codec context\n",
//                    av_get_media_type_string(type));
//            return -1;
//        }
//
//        /* Copy codec parameters from input stream to output codec context */
//       if((ret=avcodec_parameters_to_context(*dec_ctx,st->codecpar))<0){
//           LOGI("Failed to copy %s codec parameters to mDecoder context\n",
//                av_get_media_type_string(type));
//            return ret;
//        }
//
//        /* Init the decoders, with or without reference counting */
//        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
//        if((ret= avcodec_open2(*dec_ctx,dec,&opts))<0){
//            LOGI("Failed to open %s codec\n",
//                    av_get_media_type_string(type));
//            return ret;
//        }
//
//        *stream_idx= stream_index;
//    }

    return 0;
}

/** release resource */
void Decoder::release() {

}


int Decoder::decode() {
    //todo 错误处理，释放内存
    frame = av_frame_alloc();
    if (!frame) {
        release();
        return -1;
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /* read frames from the file */
     while (av_read_frame(fmt_ctx, &pkt) >=0) {
        //temp packet to store origin pkt
        AVPacket orig_pkt = pkt;
        //todo 这里无需循环解码，因为在decode内部做了循环解码
        ret = decode_packet(&got_frame, 0);
        //release AVPacket
        av_packet_unref(&orig_pkt);
    }
}


int Decoder::decode_packet(int *got_frame, int cached) {
    int ret = 0;
    int decoded = pkt.size;

    char buf[1024];

    if (pkt.stream_index == video_stream_idx) {
        /* decode video frame */
        ret = decodeVideoPacket(video_dec_ctx, frame, &pkt);
        if (ret < 0) {
            LOGI("Error decoding video frame \n");
            return ret;
        }

//        //
//        if (*got_frame) {//成功解析了一帧视频
//            //todo 变成 VideoFrame放入音频队列里面
//
//        }
    } else if (pkt.stream_index == audio_stream_idx) {
        ret = decodeVideoPacket(audio_dec_ctx, frame, &pkt);
        if (ret < 0) {
            LOGI("Error decoding video frame \n");
            return ret;
        }
    }

    return 0;

}


//todo 只包含一个压缩的AVFrame视频帧;一个音频的AVPacket可能包含多个AVFrame音频帧*/
int Decoder::decodeVideoPacket(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt) {

    int ret = 0;
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        LOGI(LOG_TAG, "Error sending a packet for decoding\n");
        return -1;
    }

    /** 返回值大于0代表还没接收，通常不会进入第二次循环，因为一个packet就代表一个视频帧*/
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame); //todo 这里的frame每次接收的时候，都会被重置，所以可以重复使用
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)//todo 一个AVPacket有可能包含多个AVFrame,所以要循环接收
            return 0;//还需要继续发送或者结束发送。 todo是否解码完成由上层方法的 read_fream 的返回值来判断。
        else if (ret < 0) {
            LOGI("Error during decoding\n");
            return -1;
        }

        LOGI("saving frame %3d\n", dec_ctx->frame_number);
//            fflush(stdout);

        /* the picture is allocated by the mDecoder. no need to
           free it */
//            LOGI(LOG_TAG,buf, sizeof(buf), "%s-%d", filename, video_dec_ctx->frame_number);



        //todo 这里代表成功接收了一帧图像,这里可以设置go_frame>=0了，在外面再去保存数据
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOGI("video frame");
            //todo 保存到视频帧保存到视频队列，从视频队列取出后，处理后显示

            mVideoFrameQueue->push(frame);
            //todo 待处理
            mRenderController->render(frame);


        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGI("audio frame");
            //todo 保存到音频队列，从音频队列取出后处理
//            dec_ctx->channel_layout;
//            dec_ctx->sample_rate;
//            dec_ctx->

            // 在解码线程之前已经进行了初始化，这里，重新采样后进行播放
            mAudioFrameQueue->push(frame);
        }
    }
    return 0;
}


//todo 视频的解码线程
void Decoder::videoThread() {
    //todo 处理后，放到最终的渲染的帧队列。在视频的渲染线线程取
    while (true){
        AVFrame * frame=mVideoFrameQueue->wait_and_pop();
        mRenderController->render(frame);
        av_frame_unref(frame);
    }

}


//todo 音频的解码线程
void Decoder::audioThread() {
    //todo 解码后，方式音频帧队列，供播放者使用
        mAudioController->start();
    while (true){
        AVFrame * frame=mVideoFrameQueue->wait_and_pop();
        //todo 进行重新采样









        av_frame_unref(frame);
    }

}

//带返回值就好了
void audioPCMQueueCallback(void *opaque, uint8_t *stream, int len) {

//    mediaPlayer->pcmQueueCallback(stream, len);
     //todo current 这里进行重新采样


}
/**
 *
 * @param wanted_channel_layout 解码出来的channel_layout
 * @param wanted_nb_channels 解码出来的通道数
 * @param wanted_sample_rate 解码出来的采样率
 */
int Decoder::openAudioController(int64_t wanted_channel_layout, int wanted_nb_channels,
                                  int wanted_sample_rate) {
    AudioDeviceSpec wanted_spec, spec;
    const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
    const int next_sample_rates[] = {44100, 48000};
    int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;//todo 采样率列表的size -1.
    if (wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)
        || !wanted_channel_layout) {
        wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
        wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
    }
    wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
    wanted_spec.channels = wanted_nb_channels;
    wanted_spec.freq = wanted_sample_rate;
    if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
        av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
        return -1;
    }
    //todo 找到第一个小于期望采样率的值
    while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq) {
        next_sample_rate_idx--;
    }

    wanted_spec.format = AV_SAMPLE_FMT_S16;//采样的格式
    wanted_spec.samples = FFMAX(AUDIO_MIN_BUFFER_SIZE,
                                2 << av_log2(wanted_spec.freq / AUDIO_MAX_CALLBACKS_PER_SEC));
    wanted_spec.callback =audioPCMQueueCallback;
    //
    //todo 这里的this 应该是player对象。
//    wanted_spec.userdata = this;

    // 打开音频设备
//    while (mAudioController->init(&wanted_spec, &spec) < 0) {
//        av_log(NULL, AV_LOG_WARNING, "Failed to open audio device: (%d channels, %d Hz)!\n",
//               wanted_spec.channels, wanted_spec.freq);
//        wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
//        if (!wanted_spec.channels) {
//            wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
//            wanted_spec.channels = wanted_nb_channels;
//            if (!wanted_spec.freq) {
//                av_log(NULL, AV_LOG_ERROR, "No more combinations to try, audio open failed\n");
//                return -1;
//            }
//        }
//        wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
//    }

    if (spec.format != AV_SAMPLE_FMT_S16) {
        av_log(NULL, AV_LOG_ERROR, "audio format %d is not supported!\n", spec.format);
        return -1;
    }

    if (spec.channels != wanted_spec.channels) {
        wanted_channel_layout = av_get_default_channel_layout(spec.channels);
        if (!wanted_channel_layout) {
            av_log(NULL, AV_LOG_ERROR, "channel count %d is not supported!\n", spec.channels);
            return -1;
        }
    }

//    // 初始化音频重采样器
    if (!audioResampler) {
        audioResampler = new AudioResampler;
    }
    // 设置需要重采样的参数
    audioResampler->setResampleParams(&spec, wanted_channel_layout);
    return 0;
}
