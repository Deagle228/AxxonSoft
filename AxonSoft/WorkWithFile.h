#pragma once

#include "Includes.h"
#include "ThreadPool.h"

class Result;

class WorkWithFile
{
public:
    WorkWithFile();

public:
    int doFirstTask(const std::filesystem::path directory);

    int doSecondTask(const std::filesystem::path directory, const char* str);

public:
    std::shared_ptr<Result> m_resultReference;

};