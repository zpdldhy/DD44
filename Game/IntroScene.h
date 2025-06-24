#pragma once
#include "IExecute.h"
#include "UIControler.h"

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
	IntroUIControler m_cUI;

private:
	int m_iPrevMenu = -1; 

};

