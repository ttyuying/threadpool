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
		//�̴߳���
		pthread_create(&managerID, NULL, manager, this);
		for (int i = 0; i < min; i++) {
			pthread_create(&threadIDs[i], NULL, worker, this);
		}
		return;
	} while (0);
	//�ͷ���Դ
	if (threadIDs)delete[]threadIDs;
	if (taskQ)delete taskQ;
}

void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true) {
		pthread_mutex_lock(&pool->mutexpool);
		//��ǰ��������Ƿ�Ϊ��
		while (pool->taskQ->taskNumber() == 0 && !pool->shutdown) {
			//���������߳�
			pthread_cond_wait(&pool->notEmpty, &pool->mutexpool);
			//�ж��Ƿ������߳�
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
