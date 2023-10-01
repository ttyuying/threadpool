#pragma once
#include<queue>
#include<pthread.h>
// 定义任务结构体
using callback = void(*)(void*);
struct Task
{
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f, void* arg)
    {
        function = f;
        this->arg = arg;
    }
    callback function;
    void* arg;
};
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    void addTask(Task task);
    void addTask(callback f, void* arg);
    Task takeTask();
    inline int taskNumber() {
        return m_taskQ.size();
    }
private:
    pthread_mutex_t m_mutex;
    std::queue<Task>m_taskQ;
};

