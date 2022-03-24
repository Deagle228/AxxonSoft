#pragma once

#include "Includes.h"

class Result;

class Counter
{
public:

    void substringCount(const std::filesystem::path& filePath, const char* str, std::shared_ptr<Result> resultReference);

    void stringCount(const std::filesystem::path& filePath, std::shared_ptr<Result> resultReference);

    void fillVector(std::ifstream& in, std::vector<char>& vectorChar);

private:
    std::vector<char> m_vectorChar;
    std::mutex m_substringCountMutex;
    std::mutex m_stringCountMutex;
};