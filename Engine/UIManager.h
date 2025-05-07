#pragma once
#include "Singleton.h"

class UIManager : public Singleton<UIManager>
{
	vector<shared_ptr<class AUIActor>> m_vUIList;

public:
	void Init();	
	void Render();
	void Destroy();

public:
	void SetRenderUIList(vector<shared_ptr<class AUIActor>> _vUIList) { m_vUIList = _vUIList; }
	void AddRenderUIList(shared_ptr<class AUIActor> _pUIActor) { m_vUIList.emplace_back(_pUIActor); }
};

