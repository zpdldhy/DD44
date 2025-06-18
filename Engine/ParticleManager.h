#pragma once
#include "Singleton.h"
#include "RenderStateManager.h"
class AParticleActor;

class ParticleManager : public Singleton<ParticleManager>
{
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
	void SetInstance(shared_ptr<AParticleActor> _pActor);
	void MakeInstance(shared_ptr<AParticleActor> _pActor);

private:
	RenderOption m_sharedOption;
	// InstanceActor
	vector<shared_ptr<class AInstance>> m_vInstanceList;
};

