//
// Created by Macro on 2020-02-14.
//

#include <__locale>
#include "AVSyncronizer.h"

AVSyncronizer::AVSyncronizer(PlayerState *playerState) {
    mPlayerState = playerState;
}

void AVSyncronizer::initDecoder() {
    mPlayerState->mAudioDecoder = new Decoder;
    mPlayerState->mVideoDecoder = new Decoder;
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



    //todo 相关声音的初始化，线程条件变量的初始化，精确的seek

    //todo openDevice
    createAudioRender();
    //===================================
    //todo 初始化解码器
    initDecoder();
    //todo 初始化video audio 和 ext 实际时间的时钟
    mPlayerState->initAllClock();
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
    AVPacket pkt1, *pkt = &pkt1;
    int64_t stream_start_time;
    int pkt_in_play_range = 0;
    int64_t pkt_ts;

    AVDictionary *opts = NULL;
    AVCodec *codec = NULL;
    int ret = 0;
    int sampleRate = 0;
    int nbChannels = 0;
    uint64_t channelLayout = 0;


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
        case AVMEDIA_TYPE_AUDIO: {
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
            int ret = mAudioController->open(codecContext->channel_layout, codecContext->channels,
                                             codecContext->sample_rate);
            //todo 初始化解码器和解码线程
            //本
            if (ret < 0) {
                //todo 重新设置同步模式为按照视频时间同步

            } else {
                mAudioController->start();
                mPlayerState->mAudioDecoder->audio_stream_idx = streamIndex;
                mPlayerState->mAudioDecoder->audio_stream = formatContext->streams[streamIndex];
//            mAudioDecoder->init(codecContext,&mPlayerState->mAudioQueue);
                mPlayerState->mAudioDecoder->init(codecContext);
                mPlayerState->mAudioDecoder->start(AVMEDIA_TYPE_AUDIO);
            }
        }
            break;
        case AVMEDIA_TYPE_VIDEO:
            mPlayerState->mVideoDecoder->video_stream_idx = streamIndex;
            mPlayerState->mVideoDecoder->video_stream = formatContext->streams[streamIndex];
            mPlayerState->mVideoDecoder->init(codecContext);
//            mVideoDecoder->init(codecContext,&mPlayerState->mVideQueue);
            mPlayerState->mVideoDecoder->start(AVMEDIA_TYPE_VIDEO);
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

///**
// * todo 以前的视频的渲染界面
// * @return
// */
//int AVSyncronizer::videoRefreshThread() {
//
////    while (!mPlayerState.abortRequest) {
////        if (remaining_time > 0.0)
////            av_usleep((int)(int64_t)(remaining_time * 1000000.0));
////        remaining_time = REFRESH_RATE;
////        if (is->show_mode != SHOW_MODE_NONE && (!is->paused || is->force_refresh))
////            video_refresh(ffp, &remaining_time);
//
//    initVideoRender();
//
//    for (;;) {
//        if (mPlayerState->mVideoDecoder->abortRequest) {
//            return -1;
//        }
//
////        if(mVideoDecoder->frameQueue->getSize()<=0){
////             //todo 停止渲染
////
////            return -1;
////        }
//        AVFrame *frame = mPlayerState->mVideoDecoder->frameQueue->wait_and_pop();
//        //todo 计算渲染时间
//        //=======================================================================
//        Frame* mFrame=mPlayerState->mVideoDecoder->mFrameQueue2->wait_and_pop();
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//        //=====================================================================
//        mRenderController->render(frame);
////
//        av_frame_free(&frame);
//    }
//
//    return 0;
//
//}

/**
 * 视频的渲染界面
 * @return
 */
int AVSyncronizer::videoRefreshThread() {
    initVideoRender();
    double remainingTime = 0.0;
    while (!mPlayerState->abort_request) {
        //todo 这里的睡眠打到延迟播放的目的
        if (remainingTime > 0.0)
            av_usleep((int) (int64_t) (remainingTime * 1000000.0));//todo 睡眠响应的时间再播放
        remainingTime = REFRESH_RATE;
        //todo 这些判断暂时不加
//        if (is->show_mode != SHOW_MODE_NONE && (!is->paused || is->force_refresh))
        videoRefresh(&remainingTime);//引用的方式传入的，改边的是实际的值
    }


    return 0;
}


/**
 * 展示video的实际方法
 * todo 如果是单纯的视频，直接延迟对应的时间播放下一帧就好了
 */

void AVSyncronizer::videoRefresh(double *remainingTime) {

    double time=0.0;
    Frame *lastFrame= nullptr, *nextFrame= nullptr;


    if (mPlayerState->mVideoDecoder->video_stream != nullptr) {
retry:
          //todo 尺寸暂时没有处理
//        if (mPlayerState->mVideoDecoder->mFrameQueue2->getSize() == 0) {
//            // nothing to do, no picture to display in the queue
//        } else {
            double currentDuration, duration, delay;
            /* dequeue the frame */
        //正在播放的帧
        lastFrame = mPlayerState->mVideoDecoder->mFrameQueue2->getLastFrame();
        //将要播放的帧，如果，没有播放，责不删除，
        nextFrame = mPlayerState->mVideoDecoder->mFrameQueue2->wait_and_get_front();
        //todo 找出下一帧

            //todo 暂时不处理，最后在进行序列号的赋值的处理
//            if (lasF->serial != is->videoq.serial) {//序列号不同，下一帧
//                frame_queue_next(&is->pictq);
//                goto retry;
//            }

//            if (lastFrame->serial != vp->serial)
//                is->frame_timer = av_gettime_relative() / 1000000.0;

            //todo 如果是第一针获取刷新时间
            if (lastFrame==NULL)
                mPlayerState->frameTimer = av_gettime_relative() / 1000000.0;

//            if (is->paused)
//                goto display;

            /* compute nominal last_duration */
            currentDuration = getDuration(lastFrame, nextFrame);
            delay = computeTargetDelay(currentDuration);

            time = av_gettime_relative() / 1000000.0;
            if (time < mPlayerState->frameTimer)
                mPlayerState->frameTimer= time;
            if (time < mPlayerState->frameTimer + delay) {
                *remainingTime = FFMIN(mPlayerState->frameTimer + delay - time, *remainingTime);
//                goto display;
                //todo 这里直接结束就好了
                return;
            }

            mPlayerState->frameTimer += delay;
            if (delay > 0 && time - mPlayerState->frameTimer > AV_SYNC_THRESHOLD_MAX)
                mPlayerState->frameTimer = time;

//            SDL_LockMutex(is->pictq.mutex);
            if (!isnan(nextFrame->pts))
                upDateVideoPts( nextFrame->pts,nextFrame->serial);//todo更新视频的时间戳
//            SDL_UnlockMutex(is->pictq.mutex);
             //todo 暂不处理
//            if (frame_queue_nb_remaining(&is->pictq) > 1) {
//                Frame *nextvp = frame_queue_peek_next(&is->pictq);
//                duration = vp_duration(is, vp, nextvp);
//                if (!is->step && (ffp->framedrop > 0 || (ffp->framedrop &&
//                                                         get_master_sync_type(is) !=
//                                                         AV_SYNC_VIDEO_MASTER)) &&
//                    time > is->frame_timer + duration) {
//                    frame_queue_next(&is->pictq);
//                    goto retry;
//                }
//            }
            //todo 字幕相关，暂不处理

//            if (is->subtitle_st) {
//                while (frame_queue_nb_remaining(&is->subpq) > 0) {
//                    sp = frame_queue_peek(&is->subpq);
//
//                    if (frame_queue_nb_remaining(&is->subpq) > 1)
//                        sp2 = frame_queue_peek_next(&is->subpq);
//                    else
//                        sp2 = NULL;
//
//                    if (sp->serial != is->subtitleq.serial
//                        || (is->vidclk.pts > (sp->pts + ((float) sp->sub.end_display_time / 1000)))
//                        || (sp2 && is->vidclk.pts > (sp2->pts + ((float) sp2->sub.start_display_time / 1000))))
//                    {
//                        if (sp->uploaded) {
//                            ffp_notify_msg4(ffp, FFP_MSG_TIMED_TEXT, 0, 0, "", 1);
//                        }
//                        frame_queue_next(&is->subpq);
//                    } else {
//                        break;
//                    }
//                }
//            }

//            frame_queue_next(&is->pictq);
//            is->force_refresh = 1;
//            //TODO 暂停
//            SDL_LockMutex(ffp->is->play_mutex);
//            if (is->step) {
//                is->step = 0;
//                if (!is->paused)
//                    stream_update_pause_l(ffp);
//            }
//            SDL_UnlockMutex(ffp->is->play_mutex);
        }
    display:
        /* display picture */
//        if (!ffp->display_disable && is->force_refresh && is->show_mode == SHOW_MODE_VIDEO &&
//            is->pictq.rindex_shown)
//            //todo 渲染
//            video_display2(ffp);
            //todo 直接调过来的，还是播放上一帧
            if(nextFrame!= nullptr)
                //todo 真正的播放完一帧，再去删除这一帧
                mRenderController->render(nextFrame->frame);
                mPlayerState->mVideoDecoder->mFrameQueue2->wait_and_pop();
                av_frame_free(&nextFrame->frame);


//    }
}


int AVSyncronizer::readThread() {

    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;
    int lowres = mPlayerState->lowres;
    AVPacket pkt1, *pkt = &pkt1;
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
    formatContext = avformat_alloc_context();
    mPlayerState->mFormtContext = formatContext;

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
    streamComponentOpen(mPlayerState, AVMEDIA_TYPE_VIDEO, videoIndex);


    //todo 打开音频组件相关
    streamComponentOpen(mPlayerState, AVMEDIA_TYPE_AUDIO, audioIndex);

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
        if (ret < 0) {
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
        if (pkt->stream_index == mPlayerState->mAudioDecoder->audio_stream_idx) {
            mPlayerState->mAudioDecoder->packetQueue->push(pkt1);
            LOGI("audio packet---");
            //todo 下面这个分支的所有条件的含义
        } else if (pkt->stream_index == mPlayerState->mVideoDecoder->video_stream_idx) {
//            packet_queue_put(&is->videoq, pkt);
            mPlayerState->mVideoDecoder->packetQueue->push(pkt1);
            LOGI("video packet---");

        } else if (pkt->stream_index == mPlayerState->mSubtitleDecoder->subtitle_stream_idx) {
//            packet_queue_put(&is->subtitleq, pkt);
            mPlayerState->mSubtitleDecoder->packetQueue->push(pkt1);
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
    mAudioController = new OpenSLESAudioController(mPlayerState);
}


void AVSyncronizer::initVideoRender() {
    mRenderController = new VideoRenderController;
    mRenderController->initEGL(mPlayerState->_window);
    mRenderController->prepareRender();
}

double AVSyncronizer::getDuration(Frame *last, Frame *next) {
    //todo 注释掉的内容暂时不处理
    if(last==NULL){//第一帧，延迟为0。
        return 0.0;
    }
//    if (current->serial == next->serial) {
    double duration = next->pts - last->pts;
//        if (isnan(duration) || duration <= 0 || du    ration > is->max_frame_duration)//todo 这里也先不处理
    if (isnan(duration) || duration <= 0)
        return last->duration;
    else
        return duration;
//    }
//    else {
//        return 0.0;
//    }

}

/**
 * 计算目标延迟
 * @param lastDuration
 * @param playerState
 * @return
 */
double AVSyncronizer::computeTargetDelay(double delay) {

    double syncThreshold, diff = 0;
    //TODO 根据主参考时间（视频时间或音频时间来确定如何计算延长时间）
    /* update delay to follow master synchronisation source */
    if (mPlayerState->getClock(&mPlayerState->mVideoClock) != AV_SYNC_VIDEO_MASTER) {
        diff = mPlayerState->getClock(&mPlayerState->mVideoClock)
               - mPlayerState->getMasterClock();

        syncThreshold = FFMAX(AV_SYNC_THRESHOLD_MIN,
                              FFMIN(AV_SYNC_THRESHOLD_MAX, delay));//todo 时间同步的阀值
        if (!isnan(diff) &&
            fabs(diff) < AV_NOSYNC_THRESHOLD) { //todo 如果大于 AV_NOSYNC_THRESHOL，发生重大错误，不进行修正
            if (diff <= -syncThreshold)//过慢 todo diff为负值，切值较大
                delay = FFMAX(0, delay + diff);
            else if (diff >= syncThreshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)//过快
                delay = delay + diff;
            else if (diff >= syncThreshold)
                delay = 2 * delay;
        }
    }

    return delay;
}

/**
 *
 * 更新视频的播放时间
 *
 */
void AVSyncronizer::upDateVideoPts( double pts, int serial) {
    /* update current video pts */
    mPlayerState->setClock(&mPlayerState->mVideoClock, pts, serial);
    mPlayerState->syncClockToSlave(&mPlayerState->mExctClock, &mPlayerState->mVideoClock);
}