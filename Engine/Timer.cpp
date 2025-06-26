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
    m_fTempTimer += m_fDeltaTime;
    if (m_fTempTimer > 1.0f)
    {
        m_fTempTimer = 0.0f;
        m_iGameFrame = static_cast<int>(GetFPS());
    }
    m_szTime = L"FPS: ";
    m_szTime += std::to_wstring(m_iGameFrame);
    m_szTime += L"\n";
    m_fTotalTime += m_fDeltaTime;
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
// test ¿ë ÁÖ¼®