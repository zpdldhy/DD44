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

class AActor;

class ObjectManager : public Singleton<ObjectManager>
{
	static UINT ActorCount;
	map<UINT, shared_ptr<AActor>> m_vActorList;
	vector<UINT> m_vActorIndexList;		// 임시 사용	
	shared_ptr<AActor> m_pCursor = nullptr;

	// Stencil Check용
	vector<shared_ptr<AActor>> m_vPreRenderActorList;		// stencil 미사용
	vector<shared_ptr<AActor>> m_vPostRenderActorList;	// stencil 사용

	// RenderMode
	static ComPtr<ID3D11Buffer> m_pRenderModeBuffer;
	RenderModeCB m_tRenderModeData;

public:
	void Init();
	void Tick();
	void Render();
	void Destroy();

public:
	void ObjectMove();
	void CollisionStabilization();

public:
	void AddActor(shared_ptr<AActor> _pActor);
	void AddActorList(vector<shared_ptr<AActor>> _vActorList);

	std::shared_ptr<class ATerrainTileActor> FindTileActor();
	void RemoveActor(std::shared_ptr<AActor> _pActor);
	void RemoveAll();
	
	shared_ptr<AActor> GetActor(UINT _iIndex);
	const map<UINT, shared_ptr<AActor>>& GetActorList() const;

	vector<UINT> GetActorIndexList() { return m_vActorIndexList; }	// 임시 사용

	// Cursor
	void SetCursorActor(shared_ptr<AActor> _pCursor);

private:
	void CheckStencilList();
	void ClearRenderList();

	void CreateRenderModeCB();
	void SetRenderMode(ERenderMode _eMode);

	// 충돌

};

