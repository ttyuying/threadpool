#include "ThreadPool.h"
#include<iostream>
#include<string.h>
#include<string>

using namespace std;
ThreadPool::ThreadPool(int min, int max)
{
	
	do {
		taskQ = new TaskQueue;
		if (taskQ == nullptr) {
			cout << "new taskQ fail......" << endl;
			break;
		}
		threadIDs = new pthread_t[max];
		if (threadIDs == nullptr) {
			cout << "new threadIDs fail....." << endl;
			break;
		}
		memset(threadIDs, 0, sizeof(pthread_t) * max);
		minNum = min;
		maxNum = max;
		busyNum = 0;
		liveNum = min;
		exitNum = 0;
		if (pthread_mutex_init(&mutexpool, NULL) != 0 || pthread_cond_init(&notEmpty, NULL) != 0) {
			cout << "mutex or condition init fail" << endl;
			break;
		}
		shutdown = false;
		//线程创建
		pthread_create(&managerID, NULL, manager, this);
		for (int i = 0; i < min; i++) {
			pthread_create(&threadIDs[i], NULL, worker, this);
		}
		return;
	} while (0);
	//释放资源
	if (threadIDs)delete[]threadIDs;
	if (taskQ)delete taskQ;
}

void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true) {
		pthread_mutex_lock(&pool->mutexpool);
		//当前任务队列是否为空
		while (pool->taskQ->taskNumber() == 0 && !pool->shutdown) {
			//阻塞工作线程
			pthread_cond_wait(&pool->notEmpty, &pool->mutexpool);
			//判断是否销毁线程
			if (pool->exitNum > 0) {
				pool->exitNum--;
				if (pool->liveNum > pool->minNum) {
					pool->liveNum--;
				}
			}
		}
	}
	return nullptr;
}
