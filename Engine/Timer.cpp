#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
    m_PrevTime = std::chrono::steady_clock::now();
}

void Timer::Update()
{
    auto now = std::chrono::steady_clock::now();
    m_fDeltaTime = std::chrono::duration<float>(now - m_PrevTime).count();
    m_PrevTime = now;
}

float Timer::GetDeltaTime() const
{
    return m_fDeltaTime;
}

float Timer::GetFPS() const
{
    if (m_fDeltaTime <= 0.0f)
        return 0.0f;
    return 1.0f / m_fDeltaTime;
}
