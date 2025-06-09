#pragma once
#include "Singleton.h"
#include "RenderStateManager.h"
#include "UGSParticleComponent.h"
class AParticleActor;

class ParticleManager : public Singleton<ParticleManager>
{
	vector<shared_ptr<AParticleActor>> m_vParticleList;
	vector<shared_ptr<AParticleActor>> m_vRenderParticleList;
	vector<shared_ptr<UGSParticleComponent>> m_vGSParticles;

	ComPtr<ID3D11RenderTargetView> m_pPrevRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	ComPtr<ID3D11DepthStencilView> m_pPrevDSV;

public:
	void Tick();
	void Render();
	void PreRender();
	void PostRender();
	void Destroy();

public:
	void SetParticleList(vector<shared_ptr<AParticleActor>> _vParticleList);
	void AddParticleList(vector<shared_ptr<AParticleActor>> _vParticleList);
	void AddUI(shared_ptr<AParticleActor> _vParticleList);
	void AddGSParticle(shared_ptr<UGSParticleComponent> _gsParticle);
private:
	RenderOption m_sharedOption;
};

