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
        decodeThread.detach();
    }else{
        this->decodeThread=std::thread(&Decoder::audioThread,this);
        decodeThread.detach();
    }
}

/**
 * 视频解码线程
 * @return
 */
int Decoder::videoThread() {
//    this_thread::sleep_for(chrono::seconds(10));
    //todo 思路有问题，应该统一用一个PacketQueue
    vFrame = av_frame_alloc();
    int ret=0;

    for(;;){
       AVPacket *pkt= packetQueue->waitAndPop();
        if(abortRequest){
            return  -1;
        }
        ret=decodePacketToFrame(pkt,vFrame);
//        if(ret<0){
//            av_frame_free(&vFrame);
//            av_packet_free(&pkt);
//            return ret;
//        }
//        av_packet_free(&pkt);

    }

    return 0;
}

/**
 * 音频解码线程
 * @return
 */
int Decoder::audioThread() {
//    this_thread::sleep_for(chrono::seconds(1));
    //todo 解码后，方式音频帧队列，供播放者使用
    aFrame=av_frame_alloc();
    AVPacket *pkt;
    int ret=0;
    for(;;){
        pkt=packetQueue->waitAndPop();
        ret=decodePacketToFrame(pkt,aFrame);
//        if(ret<0){
//            av_frame_free(&aFrame);
//            av_packet_free(&pkt);
//            return ret;
//        }

    }

    //todo 音频的对象初始化

//    mAudioController->start();
    while (true){
        AVFrame * frame=mVideoFrameQueue->wait_and_pop();
        //todo 进行重新采样









        av_frame_unref(frame);
    }
    return 0;
}

//todo 只包含一个压缩的AVFrame视频帧;一个音频的AVPacket可能包含多个AVFrame音频帧*/
int Decoder::decodePacketToFrame(AVPacket *pkt,AVFrame *frame1) {
        int ret=0;
        //todo 还有其它的条件判断
        if(abortRequest){
            return -1;
        }

        //todo 解码完会自动退出
//        if(packetQueue->getSize() <= 0){
//            return -1;
//        }


        ret = avcodec_send_packet(codecContext, pkt);
        if (ret < 0) {
            LOGI(LOG_TAG, "Error sending a packet for decoding\n");
            return  ret;
        }

        /** 返回值大于0代表还没接收，通常不会进入第二次循环，因为一个packet就代表一个视频帧*/
        //todo mark 这里有问题，第二次send 把avframe置零了,所以需要重新申请一个
        while (ret>=0) {
            AVFrame * frame =av_frame_alloc();
            ret = avcodec_receive_frame(codecContext, frame); //todo 这里的frame每次接收的时候，都会被重置，所以可以重复使用
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)//todo 一个AVPacket有可能包含多个AVFrame,所以要循环接收
                return -1;//还需要继续发送或者结束发送。 todo是否解码完成由上层方法的 read_fream 的返回值来判断。
            else if (ret < 0) {
                LOGI("Error during decoding\n");
                return -1;
            }

            LOGI("saving frame %3d %3d\n", codecContext->frame_number,codecContext->codec_type);
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
//        av_frame_unref(frame);
//            goto out;


    }

    return ret;
}

/**
 * 停止
 */
void Decoder::stop() {


}