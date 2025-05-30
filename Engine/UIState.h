#pragma once

enum class UIStateType
{
	ST_IDLE = 0,
	ST_HOVER,
	ST_ACTIVE,
	ST_SELECT,
	ST_COUNT,
};

class AUIActor;

class UIState
{
public:
	virtual ~UIState() = default;
	POINT m_ptMousePos = { 0, 0 };

public:
	virtual void ProcessState(AUIActor* _pUI) abstract;
	bool CheckCollision(Vec3 _scale, Vec3 _pos);
};

class IdleUIState : public UIState
{
public:
	virtual void ProcessState(AUIActor* _pUI) override;
};

class HoverUIState : public UIState
{
public:
	virtual void ProcessState(AUIActor* _pUI) override;
};

class ActiveUIState : public UIState
{
public:
	virtual void ProcessState(AUIActor* _pUI) override;
};

class SelectUIState : public UIState
{
public:
	virtual void ProcessState(AUIActor* _pUI) override;
};