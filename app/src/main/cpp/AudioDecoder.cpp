//
// Created by Macro on 2020/10/20.
//

#include "AudioDecoder.h"

#define LOG_TAG "audioDecoder"

void AudioDecoder::start() {
    packetQueue->start();
    this->decodeThread = std::thread(&AudioDecoder::audioThread, this);
    decodeThread.detach();
}

void AudioDecoder::stop() {


}


int AudioDecoder::decodeAudioPacket(AVPacket *packet, AVFrame *frame) {
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
//        LOGI("Error sending a packet for decoding %d  type %s\n", ret,
//             codecContext->codec_type == 0 ? "video" : "audio");

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

        //todo 写 pcm 数据 start ======
        int bytePerSample = av_get_bytes_per_sample(codecContext->sample_fmt);
        if (frame->data[0] && frame->data[1]) {
//                    //todo 这里得每个采样每个采样的拷贝
//                    // Number of samples per channel in an audio frame.
            for (int i = 0; i < codecContext->frame_size; i++) {
                outFileStream.write(reinterpret_cast<const char *>(frame->data[0]),
                                    bytePerSample);
                outFileStream.write(reinterpret_cast<const char *>(frame->data[1]),
                                    bytePerSample);
            }
        } else if (frame->data[0]) {//这里直接拷贝
            outFileStream.write(reinterpret_cast<const char *>(frame->data[0]),
                                frame->linesize[0]);
        }
        LOGD("after write pcm");

        // 写 pcm 数据 end =========

//=============================todo=== 重新计算Frame的时间戳==========
        AVRational tb = {1, 44100};//时间基
        LOGD("avrational======");
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
        av_frame_unref(frame);
        av_frame_move_ref(audioframe->frame, frame);
//                audioframe->duration = av_q2d((AVRational) {frame->nb_samples, frame->sample_rate});
//
        mFrameQueue2->push(audioframe);
        av_frame_free(&frame);

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

        }
        av_packet_free(&packet);
        return ret;
}

int AudioDecoder::audioThread() {

    //    this_thread::sleep_for(chrono::seconds(1));
    //todo 解码后，方式音频帧队列，供播放者使用
    aFrame = av_frame_alloc();

    int ret = 0;

//    AVPacket *packet = nullptr;
    outFileStream = ofstream(destDataFilePath, ios::binary);

    for (;;) {

        mutex.lock();
        AVPacket *packet = nullptr;
//        if (isPending) {
//            av_packet_move_ref(packet, pkt);
//            isPending = false;
//        } else {
        packet = packetQueue->waitAndPop();
//        }
        LOGD("before decode audio ");
        ret = decodeAudioPacket(packet, aFrame);
//        ret = decodePacketToFrame(packet, aFrame);
//        if(ret<0){
//            av_frame_free(&aFrame);
//            av_packet_free(&pkt);
//            return ret;
//        }
        mutex.unlock();

    }

    //todo 音频的对象初始化

//    mAudioController->start();
    while (true) {
        //todo 进行重新采样









    }
    return 0;
}