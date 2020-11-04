//
// Created by Macro on 2020/9/15.
//

#ifndef JJPLAYER_FRAMEQUEUE2_H
#define JJPLAYER_FRAMEQUEUE2_H

extern "C"{
#include "libavutil/frame.h"
};

#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
typedef struct Frame{
    AVFrame* frame;
    double pts;
    double duration;
    int width;
    int height;
    int format;
    int serial;
    int64_t pos;//在视频中的位置
} Frame;

class FrameQueue2 {
    std::mutex m;
    std::condition_variable condition;
    std::queue<Frame *> q;
    Frame * lastFrame= nullptr;
public:
    void push(Frame * message) {
        std::lock_guard<std::mutex> lk(m);
        q.push(message);
        condition.notify_all();
    }

    Frame * wait_and_pop() {
        std::unique_lock<std::mutex> lk(m);
        condition.wait(lk, [&] { return !q.empty(); });
        auto res = q.front();
        lastFrame=res;
        q.pop();
        //todo 在这里给最近的一个Frame 赋值
        return res;
    }

    /**
     * 只获取最近的frame
     * @return
     */
    Frame * wait_and_get_front() {
        std::unique_lock<std::mutex> lk(m);
        condition.wait(lk, [&] { return !q.empty(); });
        auto res = q.front();
        return res;
    }

    int getSize(){
        return q.size();
    }
    //如果是第一个，就是自己
    Frame* getLastFrame(){
        return lastFrame;
    }

};


#endif //JJPLAYER_FRAMEQUEUE2_H
