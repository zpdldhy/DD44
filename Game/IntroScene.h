#pragma once
#include "IExecute.h"

enum SelectMenu
{
	SM_START = 0,
	SM_OPTION,
	SM_EXIT,
	SM_COUNT,
};

class IntroScene : public IExecute
{
public:
	IntroScene() { m_type = SCENE_TYPE::GAME; }
	virtual ~IntroScene() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();

	void UpdateUIState();

private:
	vector<shared_ptr<class AUIActor>> m_vMenu;
	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	UINT m_vSelectMenu = 0;
	bool m_bSelectStartButton = false;	
	bool m_bthrowUI = false;
	bool m_bFadeIn = false;
	float m_fFadeTime = 2.f;
};

