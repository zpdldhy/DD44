#pragma once

enum class SceneStateType
{
	ST_INTRO = 0,
	ST_GAME,
	ST_COUNT,
};

class IExecute;

class SceneState
{
public:
	virtual ~SceneState() = default;
	POINT m_ptMousePos = { 0, 0 };

public:
	virtual void ProcessState(IExecute* _pUI) abstract;
};

class IntroSceneState : public SceneState
{
public:
	virtual void ProcessState(IExecute* _pUI) override;
};

class GameSceneState : public SceneState
{

public:
	virtual void ProcessState(IExecute* _pUI) override;
};