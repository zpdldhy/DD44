#pragma once
#include "Singleton.h"

class ObjectManager : public Singleton<ObjectManager>
{
	static UINT ActorCount;
	map<UINT, shared_ptr<class AActor>> m_vActorList;
	vector<shared_ptr<class AActor>> m_vRenderActorList;	// Render ���� Clear

	// Stencil Check��
	vector<shared_ptr<class AActor>> m_vPreRenderActorList;		// stencil �̻��
	vector<shared_ptr<class AActor>> m_vPostRenderActorList;	// stencil ���

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void AddActor(shared_ptr<class AActor> _pActor);
	void AddActorList(vector<shared_ptr<class AActor>> _vActorList);

	std::shared_ptr<class ATerrainTileActor> FindTileActor();
	void RemoveActor(std::shared_ptr<class AActor> _pActor);
	
	shared_ptr<class AActor> GetActor(UINT _iIndex);
	const map<UINT, shared_ptr<class AActor>>& GetActorList() const;

private:
	void CheckStencilList();
	void ClearRenderList();
};

