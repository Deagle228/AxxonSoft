#include "Counter.h"
#include "Result.h"

void Counter::fillVector(std::ifstream& in, std::vector<char>& vectorChar)
{
    char character;
    while (in.get(character))
    {
        try
        {
            vectorChar.push_back(character);
        }
        catch (const std::exception& ex)
        {
            std::cout << ex.what();
            in.close();
            abort();
        }
    }
}

void Counter::substringCount(const std::filesystem::path& filePath, const char* str, std::shared_ptr<Result> resultReference)
{
    std::unique_lock<std::mutex> lock(m_substringCountMutex);
    std::ifstream file(filePath);
    int substringCounter = 0;
    if (file.is_open())
    {
        fillVector(file, m_vectorChar);

        auto it = m_vectorChar.begin();
        for (int i = 0; i < m_vectorChar.size(); ++i)
        {
            it = std::search(it, m_vectorChar.end(), str, str + strlen(str));
            if (it != m_vectorChar.end())
            {
                ++substringCounter;
                ++it;
            }
            else
            {
                break;
            }
        }

        m_vectorChar.clear();
    }
    file.close(); 

    resultReference->setSubstringCounter(substringCounter);
}

void Counter::stringCount(const std::filesystem::path& filePath,std::shared_ptr<Result> resultReference)
{
    std::unique_lock<std::mutex> lock(m_stringCountMutex);
    std::ifstream file(filePath);
    int stringCounter = 0;
    if (file.is_open())
    {
       fillVector(file, m_vectorChar);

        for (int i = 0; i < m_vectorChar.size(); ++i)
        {
            if (m_vectorChar[i] == '\r' || m_vectorChar[i] == '\n')
            {
                ++stringCounter;
            }
        }
        ++stringCounter;

        m_vectorChar.clear();
    }
    file.close();

    resultReference->setStringCounter(stringCounter);
}

