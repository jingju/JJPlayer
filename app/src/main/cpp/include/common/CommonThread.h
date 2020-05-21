#ifndef LIVE_THREAD_H
#define LIVE_THREAD_H

#include <pthread.h>

class CommonThread {
public:
	CommonThread();
	~LiveThread();

	void start();
	void startAsync();
	int wait();

	void waitOnNotify();
	void notify();
	virtual void stop();

protected:
	bool mRunning;

	virtual void handleRun(void* ptr);

protected:
	pthread_t mThread;
	pthread_mutex_t mLock;
	pthread_cond_t mCondition;

	static void* startThread(void* ptr);
};

#endif //LIVE_THREAD_H
