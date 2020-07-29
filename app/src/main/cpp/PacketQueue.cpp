//
// Created by Macro on 2020/7/13.
//

#include "include/PacketQueue.h"

PacketQueue::PacketQueue() {

}

void PacketQueue::start() {
    std::lock_guard<std::mutex> lk(mMutex);
    abort_request=0;
    mCondition.notify_all();
    //todo 这里不需要自动解锁，因为lk的析构函数会自动解锁
    //     在方法内部的变量执行完，会自销毁，调用析构函数，
}

/**
 * 中止
 * @return
 */
int PacketQueue::abort() {
    std::lock_guard<std::mutex> lk(mMutex);
    abort_request = 1;
    mCondition.notify_all();//唤醒阻塞
}

int PacketQueue::push(AVPacket packet) {
    if(abort_request){
        return -1;
    }
    std::lock_guard<std::mutex> lk(mMutex);
    mQueue.push(packet);
    mCondition.notify_all();
    return 0;
}

/**
 * 填充空packet
 * @return
 */
int PacketQueue::pushNullPackets(int streamIndex) {
    AVPacket pkt,*pkt1=&pkt;
    av_init_packet(pkt1);
    pkt1->data = NULL;
    pkt1->size = 0;
    pkt1->stream_index = streamIndex;
    return push( pkt);
}

/**
 * 刷新
 */
void PacketQueue::flush() {
    std::lock_guard<std::mutex> lk(mMutex);
    for (int i = 0; i < mQueue.size(); ++i) {
        AVPacket packet =mQueue.front();
        mQueue.pop();
        av_packet_unref(&packet);
    }
    duration=0;
}

AVPacket* PacketQueue::waitAndPop() {
    std::unique_lock<std::mutex> lk(mMutex);
    if(abort_request){
        return nullptr;
    }
    mCondition.wait(lk, [&] { return !mQueue.empty(); });
    auto res = mQueue.front();
    mQueue.pop();
    return &res;
}


int PacketQueue::getSize() {
    std::lock_guard<std::mutex> lk(mMutex);
    int size =mQueue.size();
    return size;
}


