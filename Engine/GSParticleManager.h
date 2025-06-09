#pragma once
#include "Singleton.h"

class UGSParticleComponent;

class GSParticleManager : public Singleton<GSParticleManager>
{
public:
    void Add(const shared_ptr<UGSParticleComponent>& component);
    void Render();

private:
    vector<shared_ptr<UGSParticleComponent>> m_vComponents;
};

