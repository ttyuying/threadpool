#pragma once
#include"TaskQueue.h"
class ThreadPool
{
public:
	ThreadPool(int min ,int max);
	~ThreadPool();
	void addTask(Task task);
	int getBusyNum();//获取线程池中工作的线程个数
	int getAliveNum();//获取线程池中活着的线程个数
private:
	static void* worker(void* arg);//工作的线程(消费者线程)任务函数
	static void* manager(void* arg);//管理者线程任务函数
	void ThreadExit();//单个线程退出
private:
	TaskQueue* taskQ;
	pthread_t managerID;//管理者线程id
	pthread_t* threadIDs;//工作的线程id
	int minNum;//最小线程数量
	int maxNum;//最大线程数量
	int busyNum;//忙的线程的个数
	int liveNum;//存活的线程的个数
	int exitNum;//要销毁的线程个数
	pthread_mutex_t mutexpool;//锁整个的线程池
	//pthread_cond_t notFull;//任务队列是不是满了
	pthread_cond_t notEmpty;//任务队列是不是空了
	bool shutdown;//是否销毁线程池 1：销毁 ，0：不销毁
};

