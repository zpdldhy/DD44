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

	void UpdateUIState();

private:
	vector<shared_ptr<class AUIActor>> m_vBackGround;
	vector<shared_ptr<class AUIActor>> m_vMenu;
	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	UINT m_vSelectMenu = 0;	
	float m_vDefaultSelectY = 0.f;
	bool m_bSelectStartButton = false;	
	bool m_bSelectEndButton = false;
	bool m_bthrowUI = false;

private:
	int m_iPrevMenu = -1; 

};

