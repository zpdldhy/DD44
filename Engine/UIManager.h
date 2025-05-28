#pragma once
#include "Singleton.h"

class AUIActor;

class UIManager : public Singleton<UIManager>
{
	map<UINT, shared_ptr<AUIActor>> m_vUIList;
	vector<shared_ptr<AUIActor>> m_vRenderUIList;
	static UINT m_iUIID;

	shared_ptr<AUIActor> m_pFade = nullptr;	
	Color m_cFadeColor = { 0.f, 0.f, 0.f, 0.f };
	float m_fFadeTime = 2.f;
	bool isFadeIn = false;
	bool isFadeOut = false;
	bool isFadeWorkDone = false;
	bool isFirstFrame = false;

public:
	void Tick();
	void Render();
	void Destroy();

public:
	void AddUIList(vector<shared_ptr<AUIActor>> _vUIList);
	void AddUIList(vector<shared_ptr<class AActor>> _vUIList);
	void AddUI(shared_ptr<AUIActor> _pUIActor);

	shared_ptr<AUIActor> GetUI(UINT _iID);
	vector<shared_ptr<AUIActor>> GetUIList();

	void DeleteUI(shared_ptr<AUIActor> _pUIActor);
	void DeleteUI(UINT _iID);

	void RemoveAll();

public:
	void UpdateFade();
	void SetFadeActor(shared_ptr<AUIActor> _pFade);
	void SetFadeTime(float _time) { m_fFadeTime = _time; }
	void DoFadeIn() { isFadeIn = true; isFirstFrame = true;	}
	void DoFadeOut() { isFadeOut = true; isFirstFrame = true; }
	bool GetFadeWorkDone() { return isFadeWorkDone; }
};

