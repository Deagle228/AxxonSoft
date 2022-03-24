#pragma once

#include "WorkWithFile.h"


class Result
{
public:
    Result();

public:
    int firstTaskResult(const std::filesystem::path directory);

    int secondTaskResult(const std::filesystem::path directory, const char* str);

    void setStringCounter(int counter);

    int getStringCounter();

    void setSubstringCounter(int counter);

    int getSubstringCounter();

private:
    int m_stringCounter;
    int m_substringCounter;
};