#pragma once

#include "Includes.h"

class Counter
{
public:
    void writeFileInVector(std::ifstream& in)
    {
        char character;
        while (in.get(character))
        {
            try
            {
                m_vectorChar.push_back(character);
            }
            catch (const std::exception& ex)
            {
                std::cout << ex.what();
                in.close();
                return;
            }
        }
    }

    void substringCount(const std::filesystem::path& filePath, const char* str, std::shared_ptr<int> substringCounter)
    {
        {
            std::unique_lock<std::mutex> lock(m_substringCountMutex);
            std::ifstream in(filePath);
            if (in.is_open())
            {
                writeFileInVector(in);

                auto it = m_vectorChar.begin();
                for (int i = 0; i < m_vectorChar.size(); ++i)
                {
                    it = std::search(it, m_vectorChar.end(), str, str + strlen(str));
                    if (it != m_vectorChar.end())
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
                m_vectorChar.clear();
            }
        }
    }

    void stringCount(const std::filesystem::path& filePath, std::shared_ptr<int> resultCounter)
    {
        {
            std::unique_lock<std::mutex> lock(m_stringCountMutex);
            std::string line;
            std::ifstream in(filePath);
            if (in.is_open())
            {
                writeFileInVector(in);

                for (int i = 0; i < m_vectorChar.size(); ++i)
                {
                    if (m_vectorChar[i] == '\r' || m_vectorChar[i] == '\n')
                    {
                        ++* (resultCounter);
                    }
                }
                ++* (resultCounter);

                {
                    std::unique_lock<std::mutex> clear(m_stringCountClearMutex);
                    m_vectorChar.clear();
                }
            }
            in.close();
        }
    }

private:
    std::vector<char> m_vectorChar;
    std::mutex m_stringCountClearMutex;
    std::mutex m_substringCountClearMutex;
    std::mutex m_substringCountMutex;
    std::mutex m_stringCountMutex;
};