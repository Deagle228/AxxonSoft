#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) :
    m_numThreads(numThreads),
    m_isStop(false)
{}

void ThreadPool::queueThreads(Task task)
{
    {
        std::unique_lock<std::mutex> lock(m_lockMutex);
        m_queueTasks.emplace(std::move(task));
    }

    m_cv.notify_one();
}

void ThreadPool::startQueue()
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

void ThreadPool::stopQueue()
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

