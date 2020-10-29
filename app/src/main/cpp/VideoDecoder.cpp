//
// Created by Macro on 2020/10/20.
//

#include "VideoDecoder.h"

#define LOG_TAG "videoDecoder"

void VideoDecoder::start() {
    packetQueue->start();
    this->decodeThread = std::thread(&VideoDecoder::videoThread, this);
    decodeThread.detach();
}



void VideoDecoder::stop() {


}


int VideoDecoder::decodeVideoPacket(AVPacket *packet, AVFrame *frame) {

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
        LOGI("Error sending a packet for decoding %d  type %s\n", ret,
             codecContext->codec_type == 0 ? "video" : "audio");

        if (ret == AVERROR(EAGAIN)) {//不接受输入，必须输出完所有数据，才能重新读入
//            av_packet_move_ref(pkt, packet);
//            isPending = true;
        } else {
//            av_packet_unref(packet);
//            av_packet_free(&packet);
//            isPending = 0;
        }
//        av_packet_free(&packet);
        return -1;
    }

    /** 返回值大于0代表还没接收完，通常不会进入第二次循环，因为一个packet就代表一个视频帧*/
    //todo mark 这里有问题，第二次send 把avframe置零了,所以需要重新申请一个
    while (ret >= 0) {
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_frame_free(&frame);
//            av_packet_free(&packet);
            return -1;
        } else if (ret < 0) {
            LOGI("Error during decoding code %d  %s\n", ret,
                 codecContext->codec_type == 0 ? "video" : "audio");
            //todo 程序异常
            av_frame_free(&frame);
//            av_packet_free(&packet);
            return -1;
        }

        LOGI("saving frame %3d %3d %s\n", codecContext->frame_number, codecContext->codec_type,
             codecContext->codec_type == 0 ? "video" : "audio");
//            fflush(stdout);


        //=============write yuv data start==========
        if (outFileStream.is_open()) {
            LOGI("write yuv=====");
            int with = codecContext->width;
            int height = codecContext->height;
            int size=with*height;

            int l1 = frame->linesize[0];
            int l2 = frame->linesize[1];
            int l3 = frame->linesize[2];



//            outFileStream.write(reinterpret_cast<const char *>(frame->data[0]),
//                                size-16);
//            outFileStream.write(reinterpret_cast<const char *>(frame->data[1]),
//                                size / 4);
//            outFileStream.write(reinterpret_cast<const char *>(frame->data[2]),
//                                size / 4);

            for(int i= 0 ; i < height ; i++)
            {
                outFileStream.write(reinterpret_cast<const char *>(frame->data[0] + l1 * i), with);
            }

            for(int i= 0 ; i < height/2 ; i++)
            {
                outFileStream.write(reinterpret_cast<const char *>(frame->data[1] + l2 * i), with/2);
            }

            for(int i= 0 ; i < height/2 ; i++)
            {
                outFileStream.write(reinterpret_cast<const char *>(frame->data[2] + l3 * i), with/2);
            }

        }
        //todo  在写完的时候关闭流
        //=============write yuv data end==========


        //todo 暂时注释掉
//                    frameQueue->push(frame);

        //todo 转换成通用的数据保存起来
        Frame *videoFrame = (Frame *) malloc(sizeof(Frame));
        videoFrame->frame = av_frame_alloc();
        videoFrame->pts =
                frame->pts == AV_NOPTS_VALUE ? NAN : av_q2d(stream->time_base) *
                                                     frame->pts;//播放时间s
        videoFrame->format = frame->format;
        //todo 创建渲染对象，和自己的项目不符合

        //todo 保存对应的frame到队列中
        videoFrame->pos = frame->pkt_pos;

        av_frame_unref(videoFrame->frame);
        av_frame_move_ref(videoFrame->frame, frame);//会将原来的数据完全拷贝到新的AVFrame,之前的数据清零
        //序列号的处理
//                    vp->serial = serial;
        //todo 以下参数的处理暂时不加
//                    vp->sar = src_frame->sample_aspect_ratio;
//                    vp->bmp->sar_num = vp->sar.num;
//                    vp->bmp->sar_den = vp->sar.den;

        //todo push的时候，应该内部会重新创建变量，所以不用担心内存释放后，无值问题
        mFrameQueue2->push(videoFrame);
//                av_frame_unref(frame);
        av_frame_free(&frame);



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
    av_packet_free(&packet);
    return ret;

}

int VideoDecoder::videoThread() {

    //    this_thread::sleep_for(chrono::seconds(10));
    //todo 思路有问题，应该统一用一个PacketQueue
    vFrame = av_frame_alloc();
    int ret = 0;
    AVPacket *packet = nullptr;
    outFileStream = ofstream(destDataFilePath, ios::binary);

    for (;;) {

        mutex.lock();
//        if (isPending) {
//            av_packet_move_ref(packet, pkt);
//            isPending = false;
//        } else {
        packet = packetQueue->waitAndPop();
//        }
        if (abortRequest) {
            return -1;
        }
        LOGD("before decode video");
        ret = decodeVideoPacket(packet, vFrame);
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