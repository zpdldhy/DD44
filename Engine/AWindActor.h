#pragma once
#include "AActor.h"

class AWindActor : public AActor
{
public:
    AWindActor();
    virtual void Init() override;
    virtual void Tick() override;

private:
    Vec3 m_vVelocity;
    float m_fRandomOffset = RandomRange(0.f, 3.14f);
};
