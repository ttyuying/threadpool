#pragma once
#include"TaskQueue.h"
class ThreadPool
{
public:
	ThreadPool(int min ,int max);
	~ThreadPool();
	void addTask(Task task);
	int getBusyNum();//��ȡ�̳߳��й������̸߳���
	int getAliveNum();//��ȡ�̳߳��л��ŵ��̸߳���
private:
	static void* worker(void* arg);//�������߳�(�������߳�)������
	static void* manager(void* arg);//�������߳�������
	void ThreadExit();//�����߳��˳�
private:
	TaskQueue* taskQ;
	pthread_t managerID;//�������߳�id
	pthread_t* threadIDs;//�������߳�id
	int minNum;//��С�߳�����
	int maxNum;//����߳�����
	int busyNum;//æ���̵߳ĸ���
	int liveNum;//�����̵߳ĸ���
	int exitNum;//Ҫ���ٵ��̸߳���
	pthread_mutex_t mutexpool;//���������̳߳�
	//pthread_cond_t notFull;//��������ǲ�������
	pthread_cond_t notEmpty;//��������ǲ��ǿ���
	bool shutdown;//�Ƿ������̳߳� 1������ ��0��������
};

