//
// Created by Macro on 2020-02-14.
//
#include "common/CommonTools.h"
#define LOG_TAG "VideoDecoder"

#include "include/VideoDecoder.h"
/**打开资源 */
int VideoDecoder::openFile(char *src_filename) {

//    fmt_ctx=avformat_alloc_context();
//    AVIOInterruptCB int_cb = {interrupt_cb, this};
//    fmt_ctx->interrupt_callback = int_cb;

    /** open input file, and allocate format context */
    if(avformat_open_input(&fmt_ctx,src_filename,NULL,NULL)<0){
        LOGI(LOG_TAG,"Could not open source file %s\n", src_filename);
        return -1;
    }

    /** retrieve stream information */
    if(avformat_find_stream_info(fmt_ctx,NULL)<0){
        LOGI(LOG_TAG,"Could not find stream information\n");
        return -1;

    }

    if(open_codec_context(&video_stream_idx,&video_dec_ctx,fmt_ctx,AVMEDIA_TYPE_VIDEO) >= 0){
        video_stream=fmt_ctx->streams[video_stream_idx];


        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;

        ret = av_image_alloc(video_dst_data,video_dst_linesize,width,height,pix_fmt,1);

        if(ret<0){
            release();
        }

        video_dst_bufsize=ret;

    }


    if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
    }

    if (!audio_stream && !video_stream) {
        release();
    }

    return ret;
}

/** connect time out call back*/
//int VideoDecoder::interrupt_cb(void *ctx) {
//    //todo 解码失败的重试
//
//}


int VideoDecoder::open_codec_context(int *stream_idx, AVCodecContext **dec_ctx,
                                     AVFormatContext *fmt_ctx, enum AVMediaType type) {

    int ret,stream_index;
    AVStream *st;
    AVCodec *dec=NULL;
    AVDictionary *opts=NULL;

    ret = av_find_best_stream(fmt_ctx,type,-1,-1,NULL,NULL);

    if (ret < 0) {
        LOGI(LOG_TAG, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type),"");
        return ret;
    } else {
        stream_index = ret;
        st=fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);

        if(!dec){
                LOGI(LOG_TAG, "Failed to find %s codec\n",
                        av_get_media_type_string(type));
                return -1;
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            LOGI(LOG_TAG, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return -1;
        }

        /* Copy codec parameters from input stream to output codec context */
       if((ret=avcodec_parameters_to_context(*dec_ctx,st->codecpar))<0){
           LOGI(LOG_TAG, "Failed to copy %s codec parameters to decoder context\n",
                av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if((ret= avcodec_open2(*dec_ctx,dec,&opts))<0){
            LOGI(LOG_TAG, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }

        *stream_idx= stream_index;
    }

    return 0;
}

/** release resource */
void VideoDecoder::release() {


}



int VideoDecoder::decodeFrame(float duration) {

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
    while(av_read_frame(fmt_ctx,&pkt) > 0){
        //temp packet to store origin pkt
        AVPacket org_pkt=pkt;
        do{
            ret=decode_packet(&got_frame,0);
            //todo

        }while (pkt.size > 0);
    }




}



int VideoDecoder::decode_packet(int *got_frame, int cached) {
    int ret = 0;
    int decoded = pkt.size;

    *got_frame = 0;

    char buf[1024];

    if(pkt.stream_index==video_stream_idx){
        /* decode video frame */
        ret=decodeVideoPacket(got_frame,video_dec_ctx,frame,&pkt);
        if(ret < 0){
            LOGI(LOG_TAG, "Error decoding video frame \n");
            return ret;
        }

        //
        if(*got_frame){//成功解析了一帧视频
            //todo 变成 VideoFrame放入音频队列里面

        }
    }

}


//todo 一个AVFrame视频帧，可以包含多个AVPacket;一个音频的AVPacket可能包含多个AVFrame音频帧*/
int VideoDecoder::decodeVideoPacket(int *got_frame,AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt) {
    int ret=0;
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        LOGI(LOG_TAG, "Error sending a packet for decoding\n");
        return -1;
    }

    /** 返回值大于0代表还没接收，通常不会进入第二次循环，因为一个packet就代表一个视频帧*/
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)//todo 还需要再发送AVPacket才能凑成一帧。或者数据已经接解码完了。
            return 0;//还需要继续发送或者结束发送。 todo是否解码完成由上层方法的 read_fream 的返回值来判断。
        else if (ret < 0) {
            LOGI(LOG_TAG, "Error during decoding\n");
            return -1;
        }

        LOGI(LOG_TAG,"saving frame %3d\n", dec_ctx->frame_number);
//            fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
//            LOGI(LOG_TAG,buf, sizeof(buf), "%s-%d", filename, video_dec_ctx->frame_number);



        //todo 这里代表成功接收了一帧图像,这里可以设置go_frame>=0了，在外面再去保存数据
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
            *got_frame=1;
    }


}
