#include "WorkWithFile.h"
#include "Result.h"
#include "Counter.h"

WorkWithFile::WorkWithFile() : 
    m_resultReference(std::make_shared<Result>()) 
{}

int WorkWithFile::doFirstTask(const std::filesystem::path directory)
{
    ThreadPool threadPoolReference(std::thread::hardware_concurrency());
    Counter counterReference;
    threadPoolReference.startQueue();

    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ directory })
    {
        if (!dir_entry.is_directory())
        {
            auto dirEntry = dir_entry.path();

            threadPoolReference.queueThreads([this, &counterReference, dirEntry]
                {
                    counterReference.stringCount(dirEntry, m_resultReference);
                });
        }
    }

    threadPoolReference.stopQueue();

    return m_resultReference->getStringCounter();
}

int WorkWithFile::doSecondTask(const std::filesystem::path directory, const char* str)
{
    int filesCount = 0;
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ directory })
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

    Counter counterReference;
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ directory })
    {
        if (!dir_entry.is_directory())
        {
            auto dirEntry = dir_entry.path();

            pool.queueThreads([this, &counterReference, dirEntry, &str]
                {
                    counterReference.substringCount(dirEntry, str, m_resultReference);
                });
        }
    }

    pool.stopQueue();

    return m_resultReference->getSubstringCounter();
}