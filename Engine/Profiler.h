#pragma once
#include <chrono>
#include <iostream>

class Profiler
{
public:
    Profiler(const std::string& name);
    ~Profiler();

private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_startTime;
};

