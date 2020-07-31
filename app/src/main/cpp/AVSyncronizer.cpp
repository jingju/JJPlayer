//
// Created by Macro on 2020-02-14.
//

#include "AVSyncronizer.h"

AVSyncronizer::AVSyncronizer(PlayerState *playerState) {
    mPlayerState = playerState;
}

void AVSyncronizer::initDecoder() {
    mAudioDecoder = new Decoder;
    mVideoDecoder = new Decoder;
}

/**
 * 打开流
 */
void AVSyncronizer::streamOpen() {
    av_register_all();
    /**
    * todo 初始化 VideoState 类，记录player的一些状态
    */


    //初始化frame queue 和 packet queue
    /* start video display */
//    if (frame_queue_init(&is->pictq, &is->videoq, ffp->pictq_size, 1) < 0)//视频缓冲区
//        goto fail;
//    if (frame_queue_init(&is->subpq, &is->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)//subtile缓冲
//        goto fail;
//    if (frame_queue_init(&is->sampq, &is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)//音频缓冲
//        goto fail;
    //创建锁
    mPlayerState->initPacketQueue();

//    if (packet_queue_init(&is->videoq) < 0 ||
//        packet_queue_init(&is->audioq) < 0 ||
//        packet_queue_init(&is->subtitleq) < 0)
//        goto fail;




    //=======================
    //todo 一些播放器状态的判断



    //todo 初始化video audio 和 ext 实际时间的时钟



    //todo 相关声音的初始化，线程条件变量的初始化，精确的seek

    //todo openDevice
    createAudioRender();
    //===================================
    //todo 初始化解码器
    initDecoder();
    //todo 初始化视频刷新线程
    mVideoRefreshThread = std::thread(&AVSyncronizer::videoRefreshThread, this);
    mVideoRefreshThread.detach();
    //todo 流数据的读取，解码
    mReadThread = std::thread(&AVSyncronizer::readThread, this);
    mReadThread.detach();

}


/**
 * 打开音视频相关组建
 * todo 这个在read thread中打开
 * @return
 */
int
AVSyncronizer::streamComponentOpen(PlayerState *playerState, AVMediaType type, int streamIndex) {
    AVFormatContext *formatContext = playerState->mFormtContext;
    AVCodecContext *codecContext = nullptr;
    int lowres = playerState->lowres;
    AVPacket pkt1,*pkt=&pkt1;
    int64_t stream_start_time;
    int pkt_in_play_range = 0;
    int64_t pkt_ts;

    AVDictionary *opts = NULL;
    AVCodec *codec = NULL;
    int ret = 0;
    int sampleRate=0;
    int nbChannels=0;
    uint64_t channelLayout=0;


    /* find mDecoder for the stream */
    codec = avcodec_find_decoder(formatContext->streams[streamIndex]->codecpar->codec_id);

    if (!codec) {
        LOGI(LOG_TAG, "Failed to find %s codec\n",
             av_get_media_type_string(type));
        return -1;
    }

    /* Allocate a codec context for the mDecoder */
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        LOGI("Failed to allocate the %s codec context\n",
             av_get_media_type_string(type));
        return -1;
    }

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(codecContext,
                                             formatContext->streams[streamIndex]->codecpar)) < 0) {
        LOGI("Failed to copy %s codec parameters to mDecoder context\n",
             av_get_media_type_string(type));
        return ret;
    }

    // 设置时间基     AVStream的时间基和AVCodecContex的不一样
    av_codec_set_pkt_timebase(codecContext, formatContext->streams[streamIndex]->time_base);

    //todo lowres低分辨率解码，在option中设置
    if (lowres > av_codec_get_max_lowres(codec)) {
        av_log(codecContext, AV_LOG_WARNING,
               "The maximum value for lowres supported by the decoder is %d\n",
               av_codec_get_max_lowres(codec));
        lowres = av_codec_get_max_lowres(codec);
    }
    av_codec_set_lowres(codecContext, lowres);

#if FF_API_EMU_EDGE
    if (lowres) codecContext->flags |= CODEC_FLAG_EMU_EDGE;
#endif
    if (playerState->fast)
        codecContext->flags2 |= AV_CODEC_FLAG2_FAST;//todo 允许不合规律的加速
#if FF_API_EMU_EDGE
    if (codec->capabilities & AV_CODEC_CAP_DR1)
        codecContext->flags |= CODEC_FLAG_EMU_EDGE;
#endif

    if ((ret = avcodec_open2(codecContext, codec, NULL)) < 0) {
        goto fail;
    }

    //设置哪些数据包可以丢弃
    formatContext->streams[streamIndex]->discard = AVDISCARD_DEFAULT;
    switch (codecContext->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
#if CONFIG_AVFILTER
            //todo 是否设置了视频的转换器
            {
            AVFilterContext *sink;

            is->audio_filter_src.freq           = avctx->sample_rate;
            is->audio_filter_src.channels       = avctx->channels;
            is->audio_filter_src.channel_layout = get_valid_channel_layout(avctx->channel_layout, avctx->channels);
            is->audio_filter_src.fmt            = avctx->sample_fmt;
            SDL_LockMutex(ffp->af_mutex);
            if ((ret = configure_audio_filters(ffp, ffp->afilters, 0)) < 0) {
                SDL_UnlockMutex(ffp->af_mutex);
                goto fail;
            }
            ffp->af_changed = 0;
            SDL_UnlockMutex(ffp->af_mutex);
            sink = is->out_audio_filter;
            sample_rate    = av_buffersink_get_sample_rate(sink);
            nb_channels    = av_buffersink_get_channels(sink);
            channel_layout = av_buffersink_get_channel_layout(sink);
        }
#else
            sampleRate = codecContext->sample_rate;
            nbChannels = codecContext->channels;
            channelLayout = codecContext->channel_layout;
#endif
            // 打开音频设备。
            mAudioController->open(codecContext->channel_layout,codecContext->channels,codecContext->sample_rate);
            //todo 初始化解码器和解码线程
            mAudioDecoder->audio_stream_idx=streamIndex;
            mAudioDecoder->audio_stream=formatContext->streams[streamIndex];
//            mAudioDecoder->init(codecContext,&mPlayerState->mAudioQueue);
            mAudioDecoder->init(codecContext);
            mAudioDecoder->start(AVMEDIA_TYPE_AUDIO);
            break;
        case AVMEDIA_TYPE_VIDEO:
            mVideoDecoder->video_stream_idx=streamIndex;
            mVideoDecoder->video_stream=formatContext->streams[streamIndex];
            mVideoDecoder->init(codecContext);
//            mVideoDecoder->init(codecContext,&mPlayerState->mVideQueue);
            mVideoDecoder->start(AVMEDIA_TYPE_VIDEO);
            break;
        default:
            break;
    }
    //todo 如果不调用goto out 会顺序执行，codecContex释放，导致后续无法使用。
    goto out;
 //todo 失败时释放资源
fail:
    avcodec_free_context(&codecContext);
out:
    av_dict_free(&opts);
    return 0;
}

/**
 * 视频的渲染界面
 * @return
 */
int AVSyncronizer::videoRefreshThread() {

//    while (!mPlayerState.abortRequest) {
//        if (remaining_time > 0.0)
//            av_usleep((int)(int64_t)(remaining_time * 1000000.0));
//        remaining_time = REFRESH_RATE;
//        if (is->show_mode != SHOW_MODE_NONE && (!is->paused || is->force_refresh))
//            video_refresh(ffp, &remaining_time);

    initVideoRender();

    for (;;) {
        if(mVideoDecoder->abortRequest){
            return -1;
        }

//        if(mVideoDecoder->frameQueue->getSize()<=0){
//             //todo 停止渲染
//
//            return -1;
//        }
        AVFrame *frame=mVideoDecoder->frameQueue->wait_and_pop();

        mRenderController->render(frame);
//
        av_frame_free(&frame);
    }

    return 0;

}


int AVSyncronizer::readThread() {

    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;
    int lowres = mPlayerState->lowres;
    AVPacket pkt1,*pkt=&pkt1;
    int64_t stream_start_time;
    int pkt_in_play_range = 0;
    int64_t pkt_ts;
    //todo 一些初始化工作

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt1);
    pkt1.data = NULL;
    pkt1.size = 0;

    /**
     * todo 等待的互斥量的初始化mutex
     *      最近的video_stream的初始化
     *      最近的audio_stream的初始化。
     *      最近的字幕流的初始化
     *
     *      AVFormateContext的申请
     */
    //todo 初始化egl相关
//    initVideoRender();
    //todo 解码流程暂时注释掉
    // ========================

    int ret = 0;
    //todo 加锁
    formatContext= avformat_alloc_context();
    mPlayerState->mFormtContext=formatContext;

    //todo 下面的opotion都要添加
    /** open input file, and allocate format context */
    if ((ret = avformat_open_input(&mPlayerState->mFormtContext, mPlayerState->mSourPath, NULL,
                                   NULL)) < 0) {
//        LOGI("Could not open source file %s\n", mPlayerState->mSourPath);
        return -1;
    }

    //todo 这里的option的设置
    /** retrieve stream information */
    if (avformat_find_stream_info(mPlayerState->mFormtContext, NULL) < 0) {
        LOGI("Could not find stream information\n");
        return -1;
    }


    int videoIndex = -1;
    int audioIndex = -1;
    //todo 现在在readthread里面  打开 videothread 和 audiothread
    for (int i = 0; i < mPlayerState->mFormtContext->nb_streams; ++i) {
        if (mPlayerState->mFormtContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audioIndex == -1) {
                audioIndex = i;
            }
        } else if (mPlayerState->mFormtContext->streams[i]->codecpar->codec_type ==
                   AVMEDIA_TYPE_VIDEO) {
            if (videoIndex == -1) {
                videoIndex = i;
            }
        }
    }


//    videoIndex = av_find_best_stream(mPlayerState->mFormtContext, AVMEDIA_TYPE_VIDEO, videoIndex,
//                                     -1, NULL, 0);
//
//
//    audioIndex = av_find_best_stream(mPlayerState->mFormtContext, AVMEDIA_TYPE_AUDIO, audioIndex,
//                                     videoIndex, NULL, 0);


    if (videoIndex < 0 && audioIndex < 0) {
        LOGI("couldn't find video and audio stream !");
        return -1;
    }

    //todo 打开视频组件相关
    streamComponentOpen(mPlayerState,AVMEDIA_TYPE_VIDEO,videoIndex);


    //todo 打开音频组件相关
    streamComponentOpen(mPlayerState,AVMEDIA_TYPE_AUDIO,audioIndex);

    //todo 最后放开
//    is->show_mode = ffp->show_mode;
//#ifdef FFP_MERGE // bbc: dunno if we need this
//    if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
//        AVStream *st = ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];
//        AVCodecParameters *codecpar = st->codecpar;
//        AVRational sar = av_guess_sample_aspect_ratio(ic, st, NULL);
//        if (codecpar->width)
//            set_default_window_size(codecpar->width, codecpar->height, sar);
//    }
//#endif
    //todo 循环读取AVPacket并放到相应的队列
    for (;;) {
        //todo 一些播放器状态的处理。
        int ret = 0;
        ret = av_read_frame(formatContext, pkt);
        if(ret<0){
            //读完了，或者读取错误。
            int pb_eof = 0;
            int pb_error = 0;
//            if ((ret == AVERROR_EOF || avio_feof(ic->pb)) && !is->eof) {
//                ffp_check_buffering_l(ffp);
//                pb_eof = 1;
//                // check error later
//            }
//            if (ic->pb && ic->pb->error) {
//                pb_eof = 1;
//                pb_error = ic->pb->error;
//            }
//            if (ret == AVERROR_EXIT) {
//                pb_eof = 1;
//                pb_error = AVERROR_EXIT;
//            }
//
//            if (pb_eof) {
//                if (is->video_stream >= 0)
//                    packet_queue_put_nullpacket(&is->videoq, is->video_stream);
//                if (is->audio_stream >= 0)
//                    packet_queue_put_nullpacket(&is->audioq, is->audio_stream);
//                if (is->subtitle_stream >= 0)
//                    packet_queue_put_nullpacket(&is->subtitleq, is->subtitle_stream);
//                is->eof = 1;
//            }
//            if (pb_error) {
//                if (is->video_stream >= 0)
//                    packet_queue_put_nullpacket(&is->videoq, is->video_stream);
//                if (is->audio_stream >= 0)
//                    packet_queue_put_nullpacket(&is->audioq, is->audio_stream);
//                if (is->subtitle_stream >= 0)
//                    packet_queue_put_nullpacket(&is->subtitleq, is->subtitle_stream);
//                is->eof = 1;
//                ffp->error = pb_error;
//                av_log(ffp, AV_LOG_ERROR, "av_read_frame error: %s\n", ffp_get_error_string(ffp->error));
//                // break;
//                // break;
//            } else {
//                ffp->error = 0;
//            }
//            if (is->eof) {
//                ffp_toggle_buffering(ffp, 0);
//                SDL_Delay(100);
//            }
//            SDL_LockMutex(wait_mutex);
//            SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
//            SDL_UnlockMutex(wait_mutex);
//            ffp_statistic_l(ffp);
//            continue;
            return -1;
        } else {
//            is->eof = 0;
        }

//        if (pkt->flags & AV_PKT_FLAG_DISCONTINUITY) {
//            if (is->audio_stream >= 0) {
//                packet_queue_put(&is->audioq, &flush_pkt);
//            }
//            if (is->subtitle_stream >= 0) {
//                packet_queue_put(&is->subtitleq, &flush_pkt);
//            }
//            if (is->video_stream >= 0) {
//                packet_queue_put(&is->videoq, &flush_pkt);
//            }
//        }



        //todo 将读取的数据包放入对应的队列
        /* check if packet is in play range specified by user, then queue, otherwise discard */
        stream_start_time = formatContext->streams[pkt->stream_index]->start_time;
        pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
        //todo 渲染时间，加上
//        pkt_in_play_range = mPlayerState->duration == AV_NOPTS_VALUE ||
//                            (pkt_ts -
//                             (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
//                            av_q2d(formatContext->streams[pkt->stream_index]->time_base) -
//                            (double) (mPlayerState->startTime != AV_NOPTS_VALUE
//                                      ? mPlayerState->startTime : 0) / 1000000
//                            <= ((double) mPlayerState->duration / 1000000);
        if (pkt->stream_index == mAudioDecoder->audio_stream_idx) {
            mAudioDecoder->packetQueue->push(pkt1);
            LOGI("audio packet---");
            //todo 下面这个分支的所有条件的含义
        } else if (pkt->stream_index == mVideoDecoder->video_stream_idx) {
//            packet_queue_put(&is->videoq, pkt);
            mVideoDecoder->packetQueue->push(pkt1);
            LOGI("video packet---");

        } else if (pkt->stream_index == mSubtitleDecoder->subtitle_stream_idx) {
//            packet_queue_put(&is->subtitleq, pkt);
            mSubtitleDecoder->packetQueue->push(pkt1);
        } else {
            av_packet_unref(pkt);
        }

//        pkt_in_play_range = mPlayerState->duration == AV_NOPTS_VALUE ||
//                            (pkt_ts -
//                             (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
//                            av_q2d(formatContext->streams[pkt->stream_index]->time_base) -
//                            (double) (mPlayerState->startTime != AV_NOPTS_VALUE
//                                      ? mPlayerState->startTime : 0) / 1000000
//                            <= ((double) mPlayerState->duration / 1000000);
//        if (pkt->stream_index == mAudioDecoder->audio_stream_idx && pkt_in_play_range) {
//            mAudioDecoder->packetQueue->push(pkt);
//            //todo 下面这个分支的所有条件的含义
//        } else if (pkt->stream_index == mVideoDecoder->video_stream_idx && pkt_in_play_range
//                   && !(mVideoDecoder->video_stream &&
//                        (mVideoDecoder->video_stream->disposition & AV_DISPOSITION_ATTACHED_PIC))) {
////            packet_queue_put(&is->videoq, pkt);
//            mVideoDecoder->packetQueue->push(pkt);
//        } else if (pkt->stream_index == mSubtitleDecoder->subtitle_stream_idx &&
//                   pkt_in_play_range) {
////            packet_queue_put(&is->subtitleq, pkt);
//            mSubtitleDecoder->packetQueue->push(pkt);
//        } else {
//            av_packet_unref(pkt);
//        }







//
//        ffp_statistic_l(ffp);
//
//        if (ffp->ijkmeta_delay_init && !init_ijkmeta &&
//            (ffp->first_video_frame_rendered || !is->video_st) && (ffp->first_audio_frame_rendered || !is->audio_st)) {
//            ijkmeta_set_avformat_context_l(ffp->meta, ic);
//            init_ijkmeta = 1;
//        }

//        if (ffp->packet_buffering) {
//            io_tick_counter = SDL_GetTickHR();
//            if ((!ffp->first_video_frame_rendered && is->video_st) || (!ffp->first_audio_frame_rendered && is->audio_st)) {
//                if (abs((int)(io_tick_counter - prev_io_tick_counter)) > FAST_BUFFERING_CHECK_PER_MILLISECONDS) {
//                    prev_io_tick_counter = io_tick_counter;
//                    ffp->dcc.current_high_water_mark_in_ms = ffp->dcc.first_high_water_mark_in_ms;
//                    ffp_check_buffering_l(ffp);
//                }
//            } else {
//                if (abs((int)(io_tick_counter - prev_io_tick_counter)) > BUFFERING_CHECK_PER_MILLISECONDS) {
//                    prev_io_tick_counter = io_tick_counter;
//                    ffp_check_buffering_l(ffp);
//                }
//            }
//        }
    }

    return 0;
}

void AVSyncronizer::createAudioRender() {
    mAudioController = new OpenSLESAudioController;
}


void AVSyncronizer::initVideoRender() {
    mRenderController = new VideoRenderController;
    mRenderController->initEGL(mPlayerState->_window);
    mRenderController->prepareRender();
}

