#pragma once
#include "IExecute.h"

class IntroScene : public IExecute
{
public:
	IntroScene() { m_type = SCENE_TYPE::GAME; }
	virtual ~IntroScene();

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();

private:

};

