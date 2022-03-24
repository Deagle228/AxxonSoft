#pragma once

#include "Includes.h"

class ThreadPool
{
public:
    ThreadPool(int numThreads);

public:
    using Task = std::function<void()>;

public:
    void queueThreads(Task task);

public:
    void startQueue();

    void stopQueue();

private:
    std::vector<std::thread> m_vectorThreads;
    std::condition_variable m_cv;
    std::mutex m_lockMutex;
    bool m_isStop;
    std::queue<Task> m_queueTasks;
    int m_numThreads;
};
