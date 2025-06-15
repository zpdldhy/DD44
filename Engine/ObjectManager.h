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

	// InstanceActor
	vector<shared_ptr<class AInstance>> m_vInstanceList;

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

private:
	// Actor와 동일한 Mesh를 가지고 있다면 Instance에 추가해준다.
	// 만들어진 Instance가 없다면 생성한다.
	void SetInstance(shared_ptr<AActor> _pActor);
	void MakeInstance(shared_ptr<AActor> _pActor);
};

