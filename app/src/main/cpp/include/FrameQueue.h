//
// Created by Macro on 2020/6/29.
//

#ifndef JJPLAYER_FRAMEQUEUE_H
#define JJPLAYER_FRAMEQUEUE_H

#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#define MAX_FRAME_QUEUE_SIZE 10;
extern "C"{
    #include <libavutil/frame.h>
};
/**
 * todo 还要设置最大的容量值
 */
class FrameQueue {
    std::mutex m;
    std::condition_variable condition;
    std::queue<AVFrame *> q;
public:
    void push(AVFrame * message) {
        std::lock_guard<std::mutex> lk(m);
        q.push(message);
        condition.notify_all();
    }

    AVFrame * wait_and_pop() {
        std::unique_lock<std::mutex> lk(m);
        condition.wait(lk, [&] { return !q.empty(); });
        auto res = q.front();
        q.pop();
        return res;
    }

    int getSize(){
        return q.size();
    }
};


#endif //JJPLAYER_FRAMEQUEUE_H
