//
// Created by Macro on 2020/4/27.
//

#ifndef JJPLAYER_MESSAGEQUEUE_H
#define JJPLAYER_MESSAGEQUEUE_H

#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>

struct message_base {

    virtual ~message_base() {}
};

template<typename Msg>
struct wrraped_message : message_base {
    Msg contents;

    explicit wrraped_message(Msg const &contents_) : contents(contents_) {

    }
};

struct AVMessage{
    int what;
    int arg1;
    int arg2;
    void *obj;
    void (*free_l)(void *obj);
    struct AVMessage *next;
};

class MessageQueue {
    std::mutex m;
    std::condition_variable condition;
    std::queue<std::shared_ptr<AVMessage>> q;
public:
    void push(AVMessage const &msg) {
        std::lock_guard<std::mutex> lk(m);
        q.push(std::make_shared<AVMessage>(msg));
        condition.notify_all();
    }

    std::shared_ptr<AVMessage> wait_and_pop() {
        std::unique_lock<std::mutex> lk(m);
        condition.wait(lk, [&] { return !q.empty(); });
        auto res = q.front();
        q.pop();
        return res;
    }

};

#endif //JJPLAYER_MESSAGEQUEUE_H
