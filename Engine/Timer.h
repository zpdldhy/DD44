#pragma once
#include <chrono>

class Timer
{
public:
    void Init();
    void Update();
    float GetDeltaTime() const;
    float GetFPS() const;

private:
    std::chrono::steady_clock::time_point m_PrevTime;
    float m_fDeltaTime = 0.0f;
};
