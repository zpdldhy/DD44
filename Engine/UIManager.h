#pragma once
#include "Singleton.h"

class UIManager : public Singleton<UIManager>
{
	map<UINT, shared_ptr<class AUIActor>> m_vUIList;
	vector<shared_ptr<class AUIActor>> m_vRenderUIList;
	static UINT m_iUIID;

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void AddUIList(vector<shared_ptr<class AUIActor>> _vUIList);
	void AddUIList(vector<shared_ptr<class AActor>> _vUIList);
	void AddUI(shared_ptr<class AUIActor> _pUIActor);

	shared_ptr<class AUIActor> GetUI(UINT _iID);
	vector<shared_ptr<class AUIActor>> GetUIList();

	void DeleteUI(shared_ptr<class AUIActor> _pUIActor);
	void DeleteUI(UINT _iID);

	void RemoveAll();
};

