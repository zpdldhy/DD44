#pragma once
#include "Singleton.h"

class UIManager : public Singleton<UIManager>
{
	vector<shared_ptr<class AUIActor>> m_vUIList;
	vector<shared_ptr<class AUIActor>> m_vRenderUIList;

public:
	void Init();
	void Tick();
	void Render();
	void Destroy();

public:
	void SetUIList(vector<shared_ptr<class AUIActor>> _vUIList) { m_vUIList = _vUIList; }
	void AddUI(shared_ptr<class AUIActor> _pUIActor) { m_vUIList.emplace_back(_pUIActor); }

	void DeleteUI(shared_ptr<class AActor> _pActor);
};

