#pragma once
#include "Singleton.h"

enum class ERenderMode
{
	Default = 0,
	Silhouette = 1,
};

struct RenderModeCB
{
	int iRenderMode = 0;
	Vec3 padding = { 0.f, 0.f, 0.f };
};

class ObjectManager : public Singleton<ObjectManager>
{
	static UINT ActorCount;
	map<UINT, shared_ptr<class AActor>> m_vActorList;
	vector<shared_ptr<class AActor>> m_vRenderActorList;	// Render ���� Clear
	vector<UINT> m_vActorIndexList;		// �ӽ� ���

	// Stencil Check��
	vector<shared_ptr<class AActor>> m_vPreRenderActorList;		// stencil �̻��
	vector<shared_ptr<class AActor>> m_vPostRenderActorList;	// stencil ���

	// RenderMode
	static ComPtr<ID3D11Buffer> m_pRenderModeBuffer;
	RenderModeCB m_tRenderModeData;

public:
	void Init();
	void Tick();
	void Render();
	void Destroy();

public:
	void AddActor(shared_ptr<class AActor> _pActor);
	void AddActorList(vector<shared_ptr<class AActor>> _vActorList);

	std::shared_ptr<class ATerrainTileActor> FindTileActor();
	void RemoveActor(std::shared_ptr<class AActor> _pActor);
	void RemoveAll();
	
	shared_ptr<class AActor> GetActor(UINT _iIndex);
	const map<UINT, shared_ptr<class AActor>>& GetActorList() const;

	vector<UINT> GetActorIndexList() { return m_vActorIndexList; }	// �ӽ� ���
private:
	void CheckStencilList();
	void ClearRenderList();

	void CreateRenderModeCB();
	void SetRenderMode(ERenderMode _eMode);
};

