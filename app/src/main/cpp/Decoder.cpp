//
// Created by Macro on 2020-02-14.
//
#include "Decoder.h"
#include "player/PlayerState.h"
#include <stddef.h>
#include "common/CommonTools.h"

#define LOG_TAG "VideoDecoder"

#include "FrameQueue2.h"

Decoder::Decoder() {
    packetQueue = new PacketQueue;
    frameQueue = new FrameQueue;
    mFrameQueue2 = new FrameQueue2;
}

Decoder::~Decoder() {
    delete packetQueue;
    delete frameQueue;
    delete mFrameQueue2;
}

/** connect time out call back*/
//int Decoder::interrupt_cb(void *ctx) {
//    //todo 解码失败的重试
//
//}

/** release resource */
void Decoder::release() {

}

void Decoder::init(AVCodecContext *codecContext,const char * destDataFilePath) {
    this->codecContext = codecContext;
    this->destDataFilePath=destDataFilePath;
}


/**
 *  音视频解码通用
 */
void Decoder::start(AVMediaType type) {
    packetQueue->start();
    if (type == AVMEDIA_TYPE_VIDEO) {
        this->decodeThread = std::thread(&Decoder::videoThread, this);
        decodeThread.detach();
    } else {
        this->decodeThread = std::thread(&Decoder::audioThread, this);
        decodeThread.detach();
    }
}

/**
 * 视频解码线程
 * @return
 */
int Decoder::videoThread() {
    LOGI("video doecode thread start");
//    this_thread::sleep_for(chrono::seconds(10));
    //todo 思路有问题，应该统一用一个PacketQueue
    vFrame = av_frame_alloc();
    int ret = 0;
    AVPacket *packet = nullptr;
    ofstream outfile(destDataFilePath, ios::app);

    for (;;) {

//        mutex.lock();
//        if (isPending) {
//            av_packet_move_ref(packet, pkt);
//            isPending = false;
//        } else {
            packet = packetQueue->waitAndPop();
//        }
        if (abortRequest) {
            return -1;
        }

        ret = decodePacketToFrame(packet, vFrame,outfile);
        mutex.unlock();
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
 *
 *
 *
 * 是两个packet引用了同一个缓存空间，这样在一个使用完成释放掉缓存的时候，会造成另一个访问错误。
 * 所以扔给调用av_packet_ref将其引用计数+1，这样在释放其中一个packet的时候其引用的数据缓存就不会被释放掉，
 * 知道两个packet都被释放。
 * @return
 */
int Decoder::audioThread() {
//    this_thread::sleep_for(chrono::seconds(1));
    //todo 解码后，方式音频帧队列，供播放者使用
    aFrame = av_frame_alloc();

    int ret = 0;

    AVPacket *packet = nullptr;
    ofstream outfile(destDataFilePath, ios::app);

    for (;;) {

//        if (isPending) {
//            av_packet_move_ref(packet, pkt);
//            isPending = false;
//        } else {
        packet = packetQueue->waitAndPop();
//        }
        ret = decodePacketToFrame(packet, aFrame,outfile);
//        if(ret<0){
//            av_frame_free(&aFrame);
//            av_packet_free(&pkt);
//            return ret;
//        }

    }

    //todo 音频的对象初始化

//    mAudioController->start();
    while (true) {
        //todo 进行重新采样









    }
    return 0;
}
/**
 * 将yuv数据和pcm数据写到
 * @param packet
 * @param frame1
 * @return
//todo 只包含一个压缩的AVFrame视频帧;一个音频的AVPacket可能包含多个AVFrame音频帧*/
int Decoder::decodePacketToFrame(AVPacket *packet, AVFrame *frame1,ofstream &outFile) {
    int ret = 0;
    //todo 还有其它的条件判断
    if (abortRequest) {
        return -1;
    }

    //todo 解码完会自动退出
//        if(packetQueue->getSize() <= 0){
//            return -1;
//        }


    ret = avcodec_send_packet(codecContext, packet);

    if (ret < 0) {
        LOGI("Error sending a packet for decoding %d\n", ret);

        if (ret == AVERROR(EAGAIN)) {//不接受输入，必须输出完所有数据，才能重新读入
            if (nullptr != pkt) {
//                av_packet_move_ref(packet, pkt);
//                isPending = true;
            }
        } else {
            if (nullptr != pkt) {
//                av_packet_unref(pkt);
//                isPending = 0;
            }
        }
        return -1;
    }

    /** 返回值大于0代表还没接收完，通常不会进入第二次循环，因为一个packet就代表一个视频帧*/
    //todo mark 这里有问题，第二次send 把avframe置零了,所以需要重新申请一个
    while (ret >= 0) {
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_frame_free(&frame);
            return -1;
        } else if (ret < 0) {
            LOGI("Error during decoding code %d\n", ret);
            //todo 程序异常
            av_frame_free(&frame);
            return -1;
        }

        LOGI("saving frame %3d %3d\n", codecContext->frame_number, codecContext->codec_type);
//            fflush(stdout);

        //todo 对应的一些相关的赋值操作
        switch (codecContext->codec_type) {
            case AVMEDIA_TYPE_AUDIO: {
                //todo today 写pcm数据到文件

//=============================todo=== 重新计算Frame的时间戳==========
                LOGI("audiotype");
//                AVRational tb = (AVRational) {1,44100};//时间基
//
//                if (frame->pts != AV_NOPTS_VALUE) {
//                    //frame pts 为codecContext的时间基础，这里需要转换成codecContext的时间基
//                    frame->pts = av_rescale_q(frame->pts, av_codec_get_pkt_timebase(codecContext),
//                                              tb);//   1/xxx  /
//                } else if (next_pts != AV_NOPTS_VALUE) {
//                    frame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
//                }
//                if (frame->pts != AV_NOPTS_VALUE) {
//                    next_pts = frame->pts + frame->nb_samples;
//                    next_pts_tb = tb;
//                }
                Frame *audioframe = (Frame *) malloc(sizeof(Frame));
                audioframe->frame = av_frame_alloc();
//                audioframe->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
//                audioframe->pos = frame->pkt_pos;
////                    audioframe->serial = is->auddec.pkt_serial;
                av_frame_move_ref(audioframe->frame, frame);
//                audioframe->duration = av_q2d((AVRational) {frame->nb_samples, frame->sample_rate});
//
                mFrameQueue2->push(audioframe);
                av_frame_unref(frame);
//                av_frame_free(&frame);

//==============================todo==============================
//                    frameQueue->push(frame);
//                    //todo 转换成通用的数据保存起来
//                    Frame *audioframe = (Frame *) malloc(sizeof(Frame));
//                    audioframe->frame = av_frame_alloc();
//
////                    audioframe->pts =av_q2d(audio_stream->time_base);
////                            frame->pts == AV_NOPTS_VALUE ? NAN : av_q2d(audio_stream->time_base) *
////                                                                 frame->pts;//播放时间s
//                    //todo 音频的时间戳肯定需要处理
//                    frame_pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
//                    audioframe->format = frame->format;
////                    //todo 创建渲染对象，和自己的项目不符合
////
////                    //todo audioframe
//                    audioframe->pos = frame->pkt_pos;
//                    av_frame_move_ref(audioframe->frame, frame);
//                    //序列号的处理
////                    vp->serial = serial;
//                    //todo 以下参数的处理暂时不加
////                    vp->sar = src_frame->sample_aspect_ratio;
////                    vp->bmp->sar_num = vp->sar.num;
////                    vp->bmp->sar_den = vp->sar.den;
////
////                    //todo push的时候，应该内部会重新创建变量，所以不用担心内存释放后，无值问题
////                    mFrameQueue2->push(audioframe);
////                    av_frame_unref(frame);
                break;
            }
            case AVMEDIA_TYPE_VIDEO: {

                //todo 暂时注释掉
//                    frameQueue->push(frame);

                //todo 转换成通用的数据保存起来
                Frame *videoFrame = (Frame *) malloc(sizeof(Frame));
                videoFrame->frame = av_frame_alloc();
                videoFrame->pts =
                        frame->pts == AV_NOPTS_VALUE ? NAN : av_q2d(video_stream->time_base) *
                                                             frame->pts;//播放时间s
                videoFrame->format = frame->format;
                //todo 创建渲染对象，和自己的项目不符合

                //todo 保存对应的frame到队列中
                videoFrame->pos = frame->pkt_pos;
                av_frame_move_ref(videoFrame->frame, frame);
                //序列号的处理
//                    vp->serial = serial;
                //todo 以下参数的处理暂时不加
//                    vp->sar = src_frame->sample_aspect_ratio;
//                    vp->bmp->sar_num = vp->sar.num;
//                    vp->bmp->sar_den = vp->sar.den;

                //todo push的时候，应该内部会重新创建变量，所以不用担心内存释放后，无值问题
                mFrameQueue2->push(videoFrame);
//                av_frame_unref(frame);
//                av_frame_free(&frame);

                //todo 写yuv 数据到文件=========
                /**
                 * 请求权限
                 * 打开文件
                 * 写入 yuv 数据
                 */

                //todo today  在开始申请权限的时候，申请所有权限
                if(outFile.is_open()){
                    int with=codecContext->width;
                    int height=codecContext->height;
                    outFile.write(reinterpret_cast<const char *>(frame->data[0]), with * height);
                    outFile.write(reinterpret_cast<const char *>(frame->data[1]), with * height/4);
                    outFile.write(reinterpret_cast<const char *>(frame->data[2]), with * height/4);
                }
                av_frame_unref(frame);
                av_frame_free(&frame);

                break;
            }
            case AVMEDIA_TYPE_SUBTITLE: {
                frameQueue->push(frame);
                break;
            }

        }
//        if(pkt!= nullptr){
//            av_packet_unref(pkt);
//        }
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