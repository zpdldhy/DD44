#include "pch.h"
#include "Profiler.h"

Profiler::Profiler(const std::string& name) 
	: m_name(name), m_startTime(std::chrono::high_resolution_clock::now())
{
	
}

Profiler::~Profiler()
{
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<float>(endTime - m_startTime).count();

	char buffer[256];
	sprintf_s(buffer, "[Profiler] %s took %.6f seconds\n", m_name.c_str(), duration);
	OutputDebugStringA(buffer);
}

