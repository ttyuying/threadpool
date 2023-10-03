#include "ThreadPool.h"
#include<iostream>
#include<string.h>
#include<string>
#include <sys/types.h>
using namespace std;
template<typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{
	
	do {
		taskQ = new TaskQueue<T>;
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
template<typename T>
ThreadPool<T>::~ThreadPool()
{
	//�ر��̳߳�
	shutdown = true;
	//�������չ������߳�
	pthread_join(managerID, NULL);
	//�����������������߳�
	for (int i = 0; i < liveNum; i++) {
		pthread_cond_signal(&notEmpty);
	}
	//�ͷŶ��ڴ�
	if (taskQ) {
		delete taskQ;
	}
	if (threadIDs) {
		delete[] threadIDs;
	}
	pthread_mutex_destroy(&mutexpool);
	pthread_cond_destroy(&notEmpty);
}
template<typename T>
void ThreadPool<T>::addTask(Task<T> task)
{
	//pthread_mutex_lock(&mutexpool);������������Լ�ά����һ��������
	if (shutdown) 
	{
		//pthread_mutex_unlock(&mutexpool);
		return;
	}
	//�������
	taskQ->addTask(task);
	pthread_cond_signal(&notEmpty);

}
template<typename T>
int ThreadPool<T>::getBusyNum()
{
	pthread_mutex_lock(&mutexpool);
	int busyNum = this->busyNum;
	pthread_mutex_unlock(&mutexpool);

	return busyNum;
}
template<typename T>
int ThreadPool<T>::getAliveNum()
{
	pthread_mutex_lock(&mutexpool);
	int aliveNum = this->liveNum;
	pthread_mutex_unlock(&mutexpool);

	return aliveNum;
}
template<typename T>
void* ThreadPool<T>::worker(void* arg)
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
					pthread_mutex_unlock(&pool->mutexpool);
					pool->ThreadExit();
				}
			}
			
		}
		if (pool->shutdown) {
			pthread_mutex_unlock(&pool->mutexpool);
			pool->ThreadExit();
		}
		//�����������ȡ��һ������
		Task<T> task = pool->taskQ->takeTask();
		pool->busyNum++;
		//����
		pthread_mutex_unlock(&pool->mutexpool);
		cout << "thread " << to_string(pthread_self()) << "start working" << endl;
		task.function(task.arg);
		delete task.arg;
		task.arg = nullptr;
		cout << "thread " << to_string(pthread_self()) << "end working" << endl;
		pthread_mutex_lock(&pool->mutexpool);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexpool);
	}
	return nullptr;
}
template<typename T>
void* ThreadPool<T>::manager(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (!pool->shutdown) {
		//ÿ��3����һ��
		sleep(3);
		//ȡ���߳�������������͵�ǰ�̵߳�����,ȡ��æ���̵߳�����
		pthread_mutex_lock(&pool->mutexpool);
		int Queuesize = pool->taskQ->taskNumber();
		int liveNum = pool->liveNum;
		int busyNum = pool->busyNum;
		pthread_mutex_unlock(&pool->mutexpool);
		//����߳�
		//����ĸ���>�����̵߳ĸ���&&�����߳���<����߳���
		if (Queuesize > liveNum && liveNum < pool->maxNum) {
			pthread_mutex_lock(&pool->mutexpool);
			int counter = 0;
			for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; ++i) {
				if (pool->threadIDs[i] == 0) {
					pthread_create(&pool->threadIDs[i], NULL, worker, pool);
					counter++;
					pool->liveNum++;
				}
			}
			pthread_mutex_unlock(&pool->mutexpool);
		}
		//�����߳�,æ���߳�*2<�����߳���&&�����߳�>��С�߳���
		if (busyNum * 2 < liveNum && liveNum > pool->minNum) {
			pthread_mutex_lock(&pool->mutexpool);
			pool->exitNum = NUMBER;
			pthread_mutex_unlock(&pool->mutexpool);
			//�ù������߳���ɱ
			for (int i = 0; i < NUMBER; i++) {
				pthread_cond_signal(&pool->notEmpty);
			}
		}

	}
	return NULL;
}
template<typename T>
void ThreadPool<T>::ThreadExit()
{
	pthread_t tid = pthread_self();
	for (int i = 0; i < maxNum; i++) {
		if (threadIDs[i] == tid) {
			threadIDs[i] = 0;
			cout << "threadExit() called " << to_string(tid) << " exiting......" << endl;
			break;
		}
	}
	pthread_exit(NULL);
}
