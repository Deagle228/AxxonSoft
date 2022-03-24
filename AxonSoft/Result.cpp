#include "Result.h"

Result::Result() : 
    m_stringCounter(0),
    m_substringCounter(0)
{}

int Result::firstTaskResult(const std::filesystem::path directory)
{
    WorkWithFile m_workWithFileReference;    
    return m_workWithFileReference.doFirstTask(directory);
}

void Result::setStringCounter(int counter)
{
    m_stringCounter = counter;
}

int Result::getStringCounter()
{
    return m_stringCounter;
}

int Result::secondTaskResult(const std::filesystem::path directory, const char* str)
{   
    WorkWithFile m_workWithFileReference;
    return m_workWithFileReference.doSecondTask(directory,str);
}

void Result::setSubstringCounter(int counter)
{
    m_substringCounter = counter;
}

int Result::getSubstringCounter()
{
    return m_substringCounter;
}