#pragma once
#include "Singleton.h"

class AParticleActor;

class ParticleManager : public Singleton<ParticleManager>
{
	vector<shared_ptr<AParticleActor>> m_vParticleList;
	vector<shared_ptr<AParticleActor>> m_vRenderParticleList;

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void SetParticleList(vector<shared_ptr<AParticleActor>> _vParticleList);
	void AddParticleList(vector<shared_ptr<AParticleActor>> _vParticleList);
	void AddUI(shared_ptr<AParticleActor> _vParticleList);
};

