//
// Created by Macro on 2020-02-14.
//

#ifndef JJPLAYER_VIDEOFRAME_H
#define JJPLAYER_VIDEOFRAME_H


class VideoFrame {
private:
//    unsigned char data;
    int height;
    int width;
    float  position; //位置，用于seek功能，第一版不做
    int format;

};


#endif //JJPLAYER_VIDEOFRAME_H
