#pragma once
#include <chrono>
#include "Singleton.h"
class Timer : public Singleton<Timer>
{
public:
    void Init();
    void Update();
    float GetDeltaTime() const;
    float GetGameTime() const { return m_fTotalTime; }
    float GetFPS() const;

public:
    wstring     m_szTime;
    float       m_fTempTimer = 0.0f;
    int         m_iGameFrame = 0;
private:
    std::chrono::steady_clock::time_point m_PrevTime;
    float m_fDeltaTime = 0.0f;
    float m_fTotalTime = 0.0f;
};
