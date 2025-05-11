#pragma once
#include "Singleton.h"

class ObjectManager : public Singleton<ObjectManager>
{
	static UINT ActorCount;
	map<UINT, shared_ptr<class AActor>> m_vActorList;
	vector<shared_ptr<class AActor>> m_vRenderActorList;	

	// Stencil Check¿ë
	vector<shared_ptr<class AActor>> m_vPreRenderActorList;	
	vector<shared_ptr<class AActor>> m_vPostRenderActorList;
	vector<shared_ptr<AActor>> m_vBloomActorList;

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void AddActor(shared_ptr<class AActor> _pActor);
	void AddActorList(vector<shared_ptr<class AActor>> _vActorList);

	
	shared_ptr<class AActor> GetActor(UINT _iIndex);

private:
	void CheckStencilList();
	void ClearRenderList();
};

