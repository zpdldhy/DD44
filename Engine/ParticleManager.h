#pragma once
#include "Singleton.h"
#include "RenderStateManager.h"

enum class EParticleInstanceType
{
	PT_FIRE,
	PT_DUST,
	PT_POPDUST,
	PT_COUNT,
};

class AParticleActor;

class ParticleManager : public Singleton<ParticleManager>
{
	array<shared_ptr<class AInstance>, static_cast<size_t>(EParticleInstanceType::PT_COUNT)> m_vInstanceParticle;
	vector<shared_ptr<AParticleActor>> m_vParticleList;
	vector<shared_ptr<AParticleActor>> m_vRenderParticleList;

	ComPtr<ID3D11RenderTargetView> m_pPrevRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	ComPtr<ID3D11DepthStencilView> m_pPrevDSV;

public:
	void Tick();
	void Render();
	void PreRender();
	void PostRender();
	void Destroy();

public:
	void AddParticleList(vector<shared_ptr<AParticleActor>> _vParticleList);
	void AddUI(shared_ptr<AParticleActor> _vParticleList);

	void CreateInstance();
	void SetInstance(shared_ptr<AParticleActor> _pActor);

private:
	RenderOption m_sharedOption;
};

