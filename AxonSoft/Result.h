#pragma once

#include "ThreadPool.h"
#include "Counter.h"

class Result
{
public:
    Result(std::string directoryPath) :
        m_substringCounter(std::make_shared<int>()),
        m_stringCounter(std::make_shared<int>()),
        m_directory(directoryPath)
    {}

public:
    int firstTaskResult()
    {
        ThreadPool pool(std::thread::hardware_concurrency());
        pool.startQueue();

        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ m_directory })
        {
            if (!dir_entry.is_directory())
            {
                auto dirEntry = dir_entry.path();

                pool.queueThreads([this, dirEntry]
                    {
                        m_counterReference.stringCount(dirEntry, m_substringCounter);
                    });
            }
        }

        pool.stopQueue();

        return *(m_substringCounter);
    }

    int secondTaskResult(const char* str)
    {
        int filesCount = 0;
        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ m_directory })
        {
            if (!dir_entry.is_directory())
            {
                ++filesCount;
            }
        }
        if (filesCount > std::thread::hardware_concurrency())
        {
            filesCount = std::thread::hardware_concurrency();
        }

        ThreadPool pool(filesCount);
        pool.startQueue();

        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ m_directory })
        {
            if (!dir_entry.is_directory())
            {
                auto dirEntry = dir_entry.path();

                pool.queueThreads([this, dirEntry, &str]
                    {
                        m_counterReference.substringCount(dirEntry, str, m_substringCounter);
                    });
            }
        }

        pool.stopQueue();

        return *(m_substringCounter);
    }

private:
    std::shared_ptr<int> m_substringCounter;
    std::shared_ptr<int> m_stringCounter;
    const std::filesystem::path m_directory;
    Counter m_counterReference;
};