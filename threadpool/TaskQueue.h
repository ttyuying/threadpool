#pragma once
#include<queue>
#include<pthread.h>
// 定义任务结构体
using callback = void(*)(void*);
template<typename T>
struct Task
{
    Task<T>()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task<T>(callback f, void* arg)
    {
        function = f;
        this->arg = (T*)arg;
    }
    callback function;
    T* arg;
};
template<typename T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    void addTask(Task<T> task);
    void addTask(callback f, void* arg);
    Task<T> takeTask();
    inline size_t taskNumber() {
        return m_taskQ.size();
    }
private:
    pthread_mutex_t m_mutex;
    std::queue<Task<T>>m_taskQ;
};

