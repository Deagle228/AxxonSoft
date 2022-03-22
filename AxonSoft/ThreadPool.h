#pragma once

#include "Includes.h"

class ThreadPool
{
public:
    ThreadPool(int numThreads) :
        m_numThreads(numThreads),
        m_isStop(false)
    {}

public:
    using Task = std::function<void()>;

public:
    void queueThreads(Task task)
    {
        {
            std::unique_lock<std::mutex> lock(m_lockMutex);
            m_queueTasks.emplace(std::move(task));
        }

        m_cv.notify_one();
    }

public:
    void startQueue()
    {
        for (int i = 0; i < m_numThreads; ++i)
        {
            m_vectorThreads.emplace_back([=]
                {
                    while (true)
                    {
                        Task task;

                        {
                            std::unique_lock<std::mutex> lock(m_lockMutex);
                            m_cv.wait(lock, [=] { return m_isStop || !m_queueTasks.empty(); });

                            if (m_isStop && m_queueTasks.empty())
                            {
                                break;
                            }

                            task = std::move(m_queueTasks.front());
                            m_queueTasks.pop();
                        }

                        task();
                    }
                });
        }
    }

    void stopQueue()
    {
        {
            std::unique_lock<std::mutex> lock(m_lockMutex);
            m_isStop = true;
        }
        m_cv.notify_all();

        for (auto& thread : m_vectorThreads)
        {
            thread.join();
        }
    }

private:
    std::vector<std::thread> m_vectorThreads;
    std::condition_variable m_cv;
    std::mutex m_lockMutex;
    bool m_isStop;
    std::queue<Task> m_queueTasks;
    int m_numThreads;
};
