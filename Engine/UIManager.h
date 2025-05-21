#pragma once
#include "Singleton.h"

class UIManager : public Singleton<UIManager>
{
	vector<shared_ptr<class AUIActor>> m_vUIList;
	vector<shared_ptr<class AUIActor>> m_vRenderUIList;

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void SetUIList(vector<shared_ptr<class AUIActor>> _vUIList);
	void AddUIList(vector<shared_ptr<class AUIActor>> _vUIList);
	void AddUI(shared_ptr<class AUIActor> _pUIActor);
	vector<shared_ptr<class AUIActor>> GetUIList() { return m_vUIList; }
};

