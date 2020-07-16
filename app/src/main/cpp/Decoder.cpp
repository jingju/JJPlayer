//
// Created by Macro on 2020-02-14.
//
#include "Decoder.h"
#include "player/PlayerState.h"
#include <stddef.h>
#include "common/CommonTools.h"

#define LOG_TAG "VideoDecoder"


Decoder::Decoder() {
    packetQueue=new PacketQueue;
    frameQueue=new FrameQueue;
}

Decoder::~Decoder() {
    delete packetQueue;
    delete frameQueue;
}

/** connect time out call back*/
//int Decoder::interrupt_cb(void *ctx) {
//    //todo 解码失败的重试
//
//}

/** release resource */
void Decoder::release() {

}


int Decoder::decode() {
    //todo 这个放到音视频各自的解码线程

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

void Decoder::init(AVCodecContext *codecContext) {
    this->codecContext=codecContext;
}


/**
 *  音视频解码通用
 */
void Decoder::start(AVMediaType type) {
    packetQueue->start();
    if(type==AVMEDIA_TYPE_VIDEO){
        this->decodeThread=std::thread(&Decoder::videoThread,this);
    }else{
        this->decodeThread=std::thread(&Decoder::audioThread,this);
    }
}

/**
 * 视频解码线程
 * @return
 */
int Decoder::videoThread() {
    int ret=0;

    for(;;){
        ret=decodePacketToFrame();
        if(ret<0){
            return ret;
        }

    }

    return 0;
}

/**
 * 音频解码线程
 * @return
 */
int Decoder::audioThread() {
    //todo 解码后，方式音频帧队列，供播放者使用
    int ret=0;
    for(;;){
        ret=decodePacketToFrame();
        if(ret<0){
            return ret;
        }

    }


    mAudioController->start();
    while (true){
        AVFrame * frame=mVideoFrameQueue->wait_and_pop();
        //todo 进行重新采样









        av_frame_unref(frame);
    }
    return 0;
}


int Decoder::decodePacketToFrame() {

    AVFrame *frame=av_frame_alloc();
    AVPacket *pkt=nullptr;
    for (; ;) {

        //todo 还有其它的条件判断
        if(abortRequest){
            return -1;
        }

        //todo 解码完会自动退出
//        if(packetQueue->getSize() <= 0){
//            return -1;
//        }

        pkt= packetQueue->waitAndPop();

        int ret = 0;
        ret = avcodec_send_packet(codecContext, pkt);
        if (ret < 0) {
            LOGI(LOG_TAG, "Error sending a packet for decoding\n");
            goto fail;
        }

        /** 返回值大于0代表还没接收，通常不会进入第二次循环，因为一个packet就代表一个视频帧*/
        while (ret >= 0) {
            ret = avcodec_receive_frame(codecContext, frame); //todo 这里的frame每次接收的时候，都会被重置，所以可以重复使用
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)//todo 一个AVPacket有可能包含多个AVFrame,所以要循环接收
                return 0;//还需要继续发送或者结束发送。 todo是否解码完成由上层方法的 read_fream 的返回值来判断。
            else if (ret < 0) {
                LOGI("Error during decoding\n");
                return -1;
            }

            LOGI("saving frame %3d\n", codecContext->frame_number);
//            fflush(stdout);

            /* the picture is allocated by the mDecoder. no need to
               free it */
//            LOGI(LOG_TAG,buf, sizeof(buf), "%s-%d", filename, video_dec_ctx->frame_number);



            //todo 这里代表成功接收了一帧图像,这里可以设置go_frame>=0了，在外面再去保存数据
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);


            //todo 对应的一些相关的赋值操作
            switch (codecContext->codec_type){
                case AVMEDIA_TYPE_AUDIO:
                    frameQueue->push(frame);
                    break;
                case AVMEDIA_TYPE_VIDEO:
                    frameQueue->push(frame);
                    break;
                case AVMEDIA_TYPE_SUBTITLE:
                    frameQueue->push(frame);
                    break;
                default:
                    break;

            }
//
//            if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
//                LOGI("video frame");
//                //todo 保存到视频帧保存到视频队列，从视频队列取出后，处理后显示
//
//                mVideoFrameQueue->push(frame);
//                //todo 待处理
//                mRenderController->render(frame);
//
//
//            } else if (cod->codec_type == AVMEDIA_TYPE_AUDIO) {
//                LOGI("audio frame");
//                //todo 保存到音频队列，从音频队列取出后处理
////            dec_ctx->channel_layout;
////            dec_ctx->sample_rate;
////            dec_ctx->
//
//                // 在解码线程之前已经进行了初始化，这里，重新采样后进行播放
//                mAudioFrameQueue->push(frame);
//            }
        }

        av_packet_unref(pkt);
        av_frame_unref(frame);


    }
fail:
    av_packet_unref(pkt);
    av_frame_unref(frame);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return ret;
}

/**
 * 停止
 */
void Decoder::stop() {


}