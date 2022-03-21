#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <algorithm>
#include <functional>
#include <queue>
  

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

class Counter
{
public:
    void substringCount(const std::filesystem::path& filePath, const char* str, std::shared_ptr<int> substringCounter)
    {
        {
            std::unique_lock<std::mutex> lock(m_substringCountMutex);
            std::ifstream in(filePath);
            if (in.is_open())
            {
                char character;
                while (in.get(character))
                {
                    try
                    {
                        m_substringCountVector.push_back(character);
                    }
                    catch (const std::exception& ex)
                    {
                        std::cout << ex.what();
                        in.close();
                        return;
                    }
                }

                auto it = m_substringCountVector.begin();
                for (int i = 0; i < m_substringCountVector.size(); ++i)
                {
                    it = std::search(it, m_substringCountVector.end(), str, str + strlen(str));
                    if (it != m_substringCountVector.end())
                    {
                        ++(*substringCounter);
                        ++it;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            in.close();

            {
                std::unique_lock<std::mutex> clear(m_substringCountClearMutex);
                m_substringCountVector.clear();
            }
        }
    }

    void stringCount(const std::filesystem::path& filePath, std::shared_ptr<int> resultCounter, uintmax_t fileSize)
    {
        {
            std::unique_lock<std::mutex> lock(m_substringCountMutex);
            std::string line;
            std::ifstream in(filePath);
            bool isСarriageExist = false;
            char ch;
            if (in.is_open())
            {
                 char character;
                 while (in.get(character))
                 {
                     try
                     {
                         m_stringCountVector.push_back(character);
                     }
                     catch (const std::exception& ex)
                     {
                         std::cout << ex.what();
                         in.close();
                         return;
                     }
                 }

                 for (int i = 0; i < m_stringCountVector.size(); ++i)
                 {
                     if (m_stringCountVector[i] == '\r' || m_stringCountVector[i] == '\n')
                     {
                         ++* (resultCounter);
                     }     
                 }
                 ++* (resultCounter);
                 
                 {
                     std::unique_lock<std::mutex> clear(m_stringCountClearMutex);
                     m_stringCountVector.clear();
                 }
            }
            in.close(); 
        }
    }

private:
    std::vector<char> m_stringCountVector;
    std::vector<char> m_substringCountVector;
    std::mutex m_stringCountClearMutex;
    std::mutex m_substringCountClearMutex;
    std::mutex m_substringCountMutex;
    std::mutex m_stringCountMutex;
};

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

                auto fileSize = dir_entry.file_size();
                pool.queueThreads([this, dirEntry, fileSize]
                    {
                        m_counterReference.stringCount(dirEntry, m_substringCounter, fileSize);
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

//int main(int argc, char* argv[])
//{
//    Result resultReference(argv[1]);
//    if (argc == 2)
//    {
//        if (std::filesystem::is_directory(argv[1]))
//        {
//            
//            std::cout << "Number of strings " << resultReference.firstTaskResult();
//        }
//        else
//        {
//            std::cout << "Directory isn`t exist!";
//            return 0;
//        }
//    }
//    else if (argc == 3)
//    {
//        if (std::filesystem::is_directory(argv[1]))
//        {
//            std::cout << "Number of occurrences "<< resultReference.secondTaskResult(argv[2]);
//        }
//        else
//        {
//            std::cout << "Directory isn`t exist!";
//            return 0;
//        }
//    }
//    else
//    {
//        std::cout << "Error! Wrong count of members.";
//        return 0;
//    }
//    return 0;
//}
int main()
{
    //std::ofstream fout("cppstudio.txt"); // создаём объект класса ofstream для записи и связываем его с файлом cppstudio.txt
    //fout << "Privet\r\nPrivet"; // запись строки в файл
    //fout.close(); // закрываем файл

    Result resultReference("C:/Users/37533/Desktop/AxonSoft/text");
   // std::cout << "Number of substrings " << resultReference.secondTaskResult("\n");
    std::cout << "Number of strings " << resultReference.firstTaskResult();
}