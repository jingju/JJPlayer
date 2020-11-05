//
// Created by Macro on 2020/10/20.
//

#include "Remuxer.h"
static void log_callback_test2(void *ptr, int level, const char *fmt, va_list vl)
{
    va_list vl2;
    char *line = static_cast<char *>(malloc(128 * sizeof(char)));
    static int print_prefix = 1;
    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, 128, &print_prefix);
    va_end(vl2);
    line[127] = '\0';
    LOGE("%s", line);
    free(line);
}
Remuxer::Remuxer(PlayerState *playerState) {
    mPlayerState = playerState;
}

Remuxer::~Remuxer() {}

int Remuxer::start(const char *mediaType, const char *outFileName) {
    //todo 测试log

    av_log_set_callback(log_callback_test2);

    /**
     * 创建基本全局格式
     *
     * 分别读取本地数据
     *
     * 写如到对应的流
     *
     */
    OutputStream video_st = {0}, audio_st = {0};
    AVOutputFormat *outFmt;

    AVCodec *audio_codec, *video_codec;
    int ret;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;


    avformat_alloc_output_context2(&fmtContext, NULL, NULL, outFileName);

    if (!fmtContext) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&fmtContext, NULL, "mpeg", outFileName);
    }
    if (!fmtContext)
        return -1;//失败


    outFmt = fmtContext->oformat;

    /* Add the audio and video streams using the default format codecs
     * and initialize the codecs. */
    if (outFmt->video_codec != AV_CODEC_ID_NONE) {
        addStream(videoOutStream, &video_codec, outFmt->video_codec);
        have_video = 1;
        encode_video = 1;
    }
    if (outFmt->audio_codec != AV_CODEC_ID_NONE) {
        addStream(audioOutStream, &audio_codec, outFmt->audio_codec);
        have_audio = 1;
        encode_audio = 1;
    }

    /* Now that all the parameters are set, we can open the audio and
     * video codecs and allocate the necessary encode buffers. */
    if (have_video)
        openVideo(video_codec, opt);

    if (have_audio)
        openAudio(audio_codec, opt);

    av_dump_format(fmtContext, 0, outFileName, 1);

    /* open the output file, if needed */
    if (!(outFmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&fmtContext->pb, outFileName, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s': %s\n", outFileName,
                    av_err2str(ret));
            return 1;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(fmtContext, &opt);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file: %s\n",
                av_err2str(ret));
        return -1;
    }

    messageThread = thread(&Remuxer::messageLoop, this);
    messageThread.detach();

    if (encode_video) {
        videoWriteThread = thread(&Remuxer::videoWrite, this);
        videoWriteThread.detach();
    }
//
    if (encode_audio) {
        audioWriteThread = thread(&Remuxer::audioWrite, this);
        audioWriteThread.detach();
    }

    //todo 将读取的数据循环写入
//    while (encode_video || encode_audio) {
//        /* select the stream to encode */
////                if (encode_video &&
////                    (!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
////                                                    audio_st.next_pts, audio_st.enc->time_base) <= 0)) {//vido.next_pts before audio next
////                        encode_video = !writeVideoFrame(outFmtContex, &video_st);
////                } else {
////                        encode_audio = !writeAudioFrame(outFmtContex, &audio_st);
////                }
//
//        if (encode_video) {
//            //todo 写入视频 开启单独的线程，写视频
//
//
//        } else if (encode_audio) {
//            //todo 写入音频 开启单独的线程，写音频
//
//        }
//    }



    //todo 这里的数据留在回调函数里完成
    //======================================================================

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    //===================================================================================
    return 1;
}

void Remuxer::writeTrailer(OutputStream &video_st, OutputStream &audio_st,
                           int have_video, int have_audio) {
    av_write_trailer(fmtContext);

    /* Close each codec. */
    if (have_video)
        closeStream(fmtContext, &video_st);
    if (have_audio)
        closeStream(fmtContext, &audio_st);

    if (!(fmtContext->oformat->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&fmtContext->pb);

    /* free the stream */
    avformat_free_context(fmtContext);
}

int Remuxer::addStream(OutputStream &outSream, AVCodec **codec,
                       AVCodecID codecId) {
    //todo 测试
//    avcodec_register_all();
//   const AVCodec *codec2=NULL;
//    while((codec2=av_codec_next(codec2))!=NULL){
//        LOGI("find codec '%s'\n",codec2->name);
//    }



    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codecId);
    if (!(*codec)) {
        LOGI("Could not find encoder for '%s'\n",
             avcodec_get_name(codecId));
        //todo 这里
        return -1;
    }

    outSream.st = avformat_new_stream(fmtContext, *codec);
    if (!outSream.st) {
        fprintf(stderr, "Could not allocate stream\n");
        return -1;
    }
    outSream.st->id = fmtContext->nb_streams - 1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        return -1;
    }
    outSream.enc = c;

    switch ((*codec)->type) {//编码器类型
        case AVMEDIA_TYPE_AUDIO:
            LOGI("add stream audio");

            c->sample_fmt = (*codec)->sample_fmts ?
                            (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
            c->bit_rate = 64000;//比特率
            c->sample_rate = 44100;//采样率
            if ((*codec)->supported_samplerates) {
                c->sample_rate = (*codec)->supported_samplerates[0];
                for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                    if ((*codec)->supported_samplerates[i] == 44100)
                        c->sample_rate = 44100;
                }
            }
            //返回特定布局下的通道数
            c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
            c->channel_layout = AV_CH_LAYOUT_STEREO;
            if ((*codec)->channel_layouts) {
                c->channel_layout = (*codec)->channel_layouts[0];
                for (i = 0; (*codec)->channel_layouts[i]; i++) {//本编种类下支持的通道数
                    if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                        c->channel_layout = AV_CH_LAYOUT_STEREO;
                }
            }
            c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
            outSream.st->time_base = (AVRational) {1, c->sample_rate};//时间基设置为采样率分之一
            break;

        case AVMEDIA_TYPE_VIDEO:
            LOGI("add stream video");
            c->bit_rate = 400000;
            /* Resolution must be a multiple of two. */
            c->width = 352;
            c->height = 288;
            /* timebase: This is the fundamental unit of time (in seconds) in terms
             * of which frame timestamps are represented. For fixed-fps content,
             * timebase should be 1/framerate and timestamp increments should be
             * identical to 1. */
            outSream.st->time_base = (AVRational) {1, STREAM_FRAME_RATE};//时间基为帧率分之一
            c->time_base = outSream.st->time_base;//CodecContex的时间基，设置为流的时间基

//            c->gop_size = 12; /* emit one intra frame every twelve frames at moutStream */
            c->pix_fmt = STREAM_PIX_FMT;
            if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
                /* just for testing, we also add B-frames */
                c->max_b_frames = 2;
            }
            if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
                /* Needed to avoid using macroblocks in which some coeffs overflow.
                 * This does not happen with normal video, it just happens here as
                 * the motion of the chroma plane does not match the luma plane. */
                c->mb_decision = 2;
            }
            break;

        default:
            break;
    }

    /* Some formats want stream headers to be separate. */
    if (fmtContext->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    return 1;
}

void Remuxer::closeStream(AVFormatContext *pContext, OutputStream *outStream) {
    avcodec_free_context(&outStream->enc);
    av_frame_free(&outStream->frame);
    av_frame_free(&outStream->tmp_frame);
    sws_freeContext(outStream->sws_ctx);
    swr_free(&outStream->swr_ctx);
}

int Remuxer::openVideo(AVCodec *codec,
                       AVDictionary *optArg) {

    LOGI("open video");
    int ret;
    AVCodecContext *c = videoOutStream.enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, optArg, 0);
//    c->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    /* open the codec */
    ret = avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        return -1;
    }

    /* allocate and init a re-usable frame */
    //todo 一下内容无需啊创建，由读取的数据拷贝过来
    videoOutStream.frame = allocVideoFrame(c->pix_fmt, c->width, c->height);
    if (!videoOutStream.frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return -1;
    }
//
//        /* If the output format is not YUV420P, then a temporary YUV420P
//         * picture is needed too. It is then converted to the required
//         * output format. */
//        outStream->tmp_frame = NULL;
//        if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
//                //todo 创建临时的 AVFrame
//                outStream->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
//                if (!outStream->tmp_frame) {
//                        fprintf(stderr, "Could not allocate temporary picture\n");
//                        return -1;
//                }
//        }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(videoOutStream.st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        return -1;
    }
    return 1;
}

bool
Remuxer::writeAudioFrame(AVFormatContext *fmtContext, AVFrame *frame) {
    AVCodecContext *c;
    AVPacket pkt = {0}; // data and size must be 0;
    int ret;
    int got_packet;
    int dst_nb_samples;

    av_init_packet(&pkt);

    c = audioOutStream.enc;

    //todo 这里根据设置好的期望的数值进行重新采样

//    if (frame) {
//        /* convert samples from native format to destination codec format, using the resampler */
//        /* compute destination number of samples */
//        dst_nb_samples = av_rescale_rnd(
//                swr_get_delay(audioOutStream.swr_ctx, c->sample_rate) + frame->nb_samples,
//                c->sample_rate, c->sample_rate, AV_ROUND_UP);
//        av_assert0(dst_nb_samples == frame->nb_samples);
//
//        /* when we pass a frame to the encoder, it may keep a reference to it
//         * internally;
//         * make sure we do not overwrite it here
//         */
//        ret = av_frame_make_writable(audioOutStream.frame);
//        if (ret < 0)
//            return false;
//
//        /* convert to destination format */
//        ret = swr_convert(audioOutStream.swr_ctx,
//                          audioOutStream.frame->data, dst_nb_samples,
//                          (const uint8_t **) frame->data, frame->nb_samples);
//        if (ret < 0) {
//            LOGI("Error while converting\n");
////            fprintf(stderr, "Error while converting\n");
//            return false;
//        }
//        frame = audioOutStream.frame;
//
//        frame->pts = av_rescale_q(audioOutStream.samples_count, (AVRational) {1, c->sample_rate},
//                                  c->time_base);
//        audioOutStream.samples_count += dst_nb_samples;
//    }
    //todo 这里替换成新的api

    //====todo start=========encode audio==================

//    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);


    /* send the frame for encoding */
    ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        LOGI("Error sending the frame to the encoder\n");
        return false;
    }

    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0) {
        ret = avcodec_receive_packet(c, &pkt);
        if (ret == AVERROR(EAGAIN)) {
            isPendingAudio = true;
            return false;
        } else if (ret < 0) {
            LOGI("Error encoding audio frame\n");
            return false;
        }

        ret = writeFrame(&c->time_base, audioOutStream.st, &pkt);
        LOGI("write audio frame success");
        return true;
    }

    //====todo start=========encode audio =================
    return true;
}

int Remuxer::openAudio(AVCodec *codec,
                       AVDictionary *optArg) {
    LOGI("open audio");

    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;

    c = audioOutStream.enc;

    /* open it */
    av_dict_copy(&opt, optArg, 0);
    ret = avcodec_open2(c, codec, &opt);//打开编码器
    av_dict_free(&opt);
    if (ret < 0) {
        LOGI("Could not open audio codec: %s\n", av_err2str(ret));
        return -1;
    }


    //todo ？？？？？？？？？========
    /* init signal generator  一下数据没用，这是为了自动生成音频*/
//        outStream->t     = 0;
//        outStream->tincr = 2 * M_PI * 110.0 / c->sample_rate;
//        /* increment frequency by 110 Hz per second */
//        outStream->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
        nb_samples = 10000;
    else
        nb_samples = c->frame_size;

    // todo ？？？？？？？？？========限免根据实际情况去处理

    audioOutStream.frame = allocAudioFrame(c->sample_fmt, c->channel_layout,
                                           c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(audioOutStream.st->codecpar, c);
    if (ret < 0) {
        LOGI("Could not copy the stream parameters\n");
        return -1;
    }

    /* create resampler context */
    audioOutStream.swr_ctx = swr_alloc();
    if (!audioOutStream.swr_ctx) {
        LOGI("Could not allocate resampler context\n");
        return -1;
    }

    /* set options */
    av_opt_set_int(audioOutStream.swr_ctx, "in_channel_count", c->channels, 0);
    av_opt_set_int(audioOutStream.swr_ctx, "in_sample_rate", c->sample_rate, 0);
    av_opt_set_sample_fmt(audioOutStream.swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(audioOutStream.swr_ctx, "out_channel_count", c->channels, 0);
    av_opt_set_int(audioOutStream.swr_ctx, "out_sample_rate", c->sample_rate, 0);
    av_opt_set_sample_fmt(audioOutStream.swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

    /* initialize the resampling context */
    if ((ret = swr_init(audioOutStream.swr_ctx)) < 0) {
        LOGI("Failed to initialize the resampling context\n");
        return -1;
    }
    return 1;
}


void Remuxer::changeState(int stateCode) {
    AVMessage *message = new AVMessage;
    message->what = stateCode;
    messageQueue.push(message);//准备阶段的message
}


AVMessage *Remuxer::getMessage() {
    return messageQueue.wait_and_pop();
}

void Remuxer::messageLoop() {
    int haseVideo = -1;
    int haseAudio = -1;
    while (true) {
        if (isStop) {//结束
            return;
        }
        //todo 读取消息
        AVMessage *message = getMessage();
        int status = message->what;
        switch (status) {
            case END_OF_FILE:
                LOGI("file read finish");
                abortEncode=1;
                break;
            case VIDEO_WRITE_SUCCESS:
                LOGI("video write success");
                haseVideo = 1;
                if (haseAudio) {
                    writeTrailer(videoOutStream, audioOutStream, haseVideo, haseAudio);
                }

                break;
            case VIDEO_WRITE_ERROR:
                LOGI("vido write error");
                if (haseAudio > -1) {
                    if (haseAudio) {
                        writeTrailer(videoOutStream, audioOutStream, haseVideo, haseAudio);
                    }
                }

                break;
            case AUDIO_WRITE_SUCCESS:
                LOGI("audio write success");
                haseAudio = 1;
                if (haseVideo) {
                    writeTrailer(videoOutStream, audioOutStream, haseVideo, haseAudio);
                }
                break;
            case AUDIO_WRITE_ERROR:
                LOGI("audio  write error");
                if (haseVideo > -1) {
                    if (haseVideo) {
                        writeTrailer(videoOutStream, audioOutStream, haseVideo, haseAudio);
                    }
                }
                break;
        }
    }
}

/**
 * 读取本地并写入视频
 */
bool Remuxer::videoWrite() {
    LOGD("start write video ");
    ifstream inStream(videoPath, ios::binary);
    if (inStream.is_open()) {
        while (true) {
            readWrite:
            //todo 从frame队列中取数据，然后编码
            if(mPlayerState->endOfFile){
                if(mPlayerState->mVideoDecoder->mEncodeFrameQueue->getSize()==0){
                    changeState(VIDEO_WRITE_SUCCESS);
                    return true;
                }
            }
            LOGI("write video frame");
            AVFrame *frame = mPlayerState->mVideoDecoder->mEncodeFrameQueue->wait_and_pop();
            bool ret = writeVideoFrame(frame);
            if (!ret) {
                if (isPendingVideo) {//
                    isPendingVideo = false;
                    av_frame_free(&frame);
                    goto readWrite;
                } else {
                    av_frame_free(&frame);
                    changeState(VIDEO_WRITE_ERROR);
                    return false;
                }
            }

            av_frame_free(&frame);
        }
    } else {
        changeState(VIDEO_WRITE_ERROR);
        return false;
    }
}

/**
 * 创建一个视频AVFrame
 * @return
 */
AVFrame *Remuxer::allocVideoFrame(enum AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    int ret = av_frame_get_buffer(picture, 32);

    if (ret < 0) {
        LOGI("Could not allocate frame data.\n");
        return NULL;
    }

    return picture;
}

/**
 * 创建一个audio的AVFrame
 *
 * @param sample_fmt
 * @param channel_layout
 * @param sample_rate
 * @param nb_samples
 * @return
 */
AVFrame *Remuxer::allocAudioFrame(enum AVSampleFormat sample_fmt, uint64_t channel_layout,
                                  int sample_rate, int nb_samples) {

    AVFrame *frame = av_frame_alloc();
    int ret;

    if (!frame) {
        LOGI("Error allocating an audio frame\n");
        return NULL;
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if (nb_samples) {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            LOGI("Error allocating an audio buffer\n");
            return NULL;
        }
    }

    return frame;

}


/**
 * 读取本地并写入音频
 */
bool Remuxer::audioWrite() {
    LOGD("start write audio ");

    AVCodecContext *codecContext = mPlayerState->mAudioDecoder->codecContext;
    //todo 这里有问题，不应改有路径，直接在流上写
    ifstream inStream(audioPath, ios::binary);
    if (inStream.is_open()) {
        while (true) {
            audioWrite:
            if(mPlayerState->endOfFile){
                if(mPlayerState->mVideoDecoder->mEncodeFrameQueue->getSize()==0){
                    changeState(VIDEO_WRITE_SUCCESS);
                    return true;
                }
            }
            AVFrame *frame = mPlayerState->mVideoDecoder->mEncodeFrameQueue->wait_and_pop();
            frame->pkt_size;
            LOGI("write audio frame");
            bool ret = writeAudioFrame(fmtContext, frame);
            if (!ret) {
                if (isPendingAudio) {//
                    isPendingAudio = false;
                    av_frame_free(&frame);
                    goto audioWrite;
                } else {
                    changeState(AUDIO_WRITE_ERROR);
                    av_frame_free(&frame);
                    return false;
                }
            }
            av_frame_free(&frame);
        }
    } else {
        changeState(AUDIO_WRITE_ERROR);
        return false;
    }
}


void Remuxer::setVideoPaht(const char *videoPath) {
    this->videoPath = videoPath;
}

void Remuxer::setAudioPath(const char *audioPath) {
    this->audioPath = audioPath;
}

void Remuxer::setOutputPath(const char *outPuPath) {
    this->outPutPath = outPutPath;
}

/**
 * 写一帧
 */
bool
Remuxer::writeVideoFrame(AVFrame *frame) {
    int ret = 0;
    AVCodecContext *c;
    int got_packet = 0;
    AVPacket pkt = {0};

    c = videoOutStream.enc;

    av_init_packet(&pkt);

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
//    if (av_frame_make_writable(videoOutStream.frame) < 0)
//        return false;

//    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
    /* as we only generate a YUV420P picture, we must convert it
     * to the codec pixel format if needed */
//    if (!videoOutStream.sws_ctx) {
//        videoOutStream.sws_ctx = sws_getContext(c->width, c.->height,
//                                                mPlayerState->mVideoDecoder->pix_fmt,
//                                                c->width, c->height,
//                                                c->pix_fmt,
//                                                SCALE_FLAGS, NULL, NULL, NULL);
//        if (!videoOutStream.sws_ctx) {
//            fprintf(stderr,
//                    "Could not initialize the conversion context\n");
//            return false;
//        }
//    }
//    sws_scale(videoOutStream.sws_ctx, (const uint8_t *const *) frame->data,
//              frame->linesize, 0, c->height, videoOutStream.frame->data,
//              videoOutStream.frame->linesize);
//    } else {
//        fill_yuv_image(ost->frame, ost->next_pts, c->width, c->height);
//
//    }


//    videoOutStream..frame->pts = videoOutStream..next_pts++;



    /* encode the image */
    //=======todo ======编码流程====start======
    ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        LOGI("Error sending a frame for encoding (ret= %5d)\n", ret);
        //todo 返回错误进行处理
        return false;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(c, &pkt);
        if (ret == AVERROR(EAGAIN)) {
            isPendingVideo = true;
            return false;
        } else if (ret < 0) {
            LOGI("Error during encoding\n");
            return false;
        }
        printf("Write packet %3" PRId64" (size=%5d)\n", pkt.pts, pkt.size);
        //todo  写数据
        ret = writeFrame(&c->time_base, videoOutStream.st, &pkt);
        LOGI("write video frame success");
        return true;

    }
    //=======todo======编码流程====end======
    //todo  暂时不处理返回值
    return true;
}

int Remuxer::writeFrame(AVRational *timeBase, AVStream *stream,
                        AVPacket *pkt) {
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *timeBase, stream->time_base);
    pkt->stream_index = stream->index;

    /* Write the compressed frame to the media file. */
//    log_packet(fmtContext, pkt);
    return av_interleaved_write_frame(fmtContext, pkt);
    return 0;
}
