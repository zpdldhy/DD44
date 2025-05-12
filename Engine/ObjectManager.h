#pragma once
#include "Singleton.h"

class ObjectManager : public Singleton<ObjectManager>
{
	static UINT ActorCount;
	map<UINT, shared_ptr<class AActor>> m_vActorList;
	vector<shared_ptr<class AActor>> m_vRenderActorList;	// Render 이후 Clear

	// Stencil Check용
	vector<shared_ptr<class AActor>> m_vPreRenderActorList;		// stencil 미사용
	vector<shared_ptr<class AActor>> m_vPostRenderActorList;	// stencil 사용

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void AddActor(shared_ptr<class AActor> _pActor);
	void AddActorList(vector<shared_ptr<class AActor>> _vActorList);
	
	shared_ptr<class AActor> GetActor(UINT _iIndex);
	vector<shared_ptr<class AActor>>& GetActorList();

private:
	void CheckStencilList();
	void ClearRenderList();
};

