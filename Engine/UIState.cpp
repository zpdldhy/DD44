#include "pch.h"
#include "UIState.h"
#include "AUIActor.h"
#include "Input.h"
#include "CollisionManager.h"

bool UIState::CheckCollision(Vec3 _scale, Vec3 _pos)
{
	POINT ptMousePos = INPUT->GetMousePos();
	ptMousePos.x = (ptMousePos.x - g_windowSize.x / 2);
	ptMousePos.y = -(ptMousePos.y - g_windowSize.y / 2);

	Vec2 vMin = { -0.5f, -0.5f };
	Vec2 vMax = { +0.5f, +0.5f };

	vMin = vMin * Vec2(_scale) + Vec2(_pos);
	vMax = vMax * Vec2(_scale) + Vec2(_pos);

	return Collision2D::CheckRectToPoint(ptMousePos, vMin, vMax);
}

//-------------------------------------- Default State ----------------------------------------

void IdleUIState::ProcessState(AUIActor* _pUI)
{	
	if (CheckCollision(_pUI->GetScale(), _pUI->GetPosition()))
	{
		_pUI->SetState(make_shared<HoverUIState>());
		_pUI->SetStateType(UIStateType::ST_HOVER);
	}
}

//-------------------------------------- Hover State ----------------------------------------

void HoverUIState::ProcessState(AUIActor* _pUI)
{
	if (!CheckCollision(_pUI->GetScale(), _pUI->GetPosition()))
	{
		_pUI->SetState(make_shared<IdleUIState>());
		_pUI->SetStateType(UIStateType::ST_IDLE);
		return;
	}

	if (INPUT->GetButton(LCLICK))
	{
		_pUI->SetState(make_shared<ActiveUIState>());
		_pUI->SetStateType(UIStateType::ST_ACTIVE);
	}
}

//-------------------------------------- Active State ----------------------------------------

void ActiveUIState::ProcessState(AUIActor* _pUI)
{
	if (INPUT->GetButton(LCLICK) || INPUT->GetButtonDown(LCLICK))
		return;

	if (INPUT->GetButtonFree(UP)) 
	{
		if (CheckCollision(_pUI->GetScale(), _pUI->GetPosition()))
		{
			_pUI->SetState(make_shared<SelectUIState>());
			_pUI->SetStateType(UIStateType::ST_SELECT);
			return;
		}
		else
		{
			_pUI->SetState(make_shared<IdleUIState>());
			_pUI->SetStateType(UIStateType::ST_IDLE);
			return;
		}
	}
}

//-------------------------------------- Select State ----------------------------------------

void SelectUIState::ProcessState(AUIActor* _pUI)
{
	if (!CheckCollision(_pUI->GetScale(), _pUI->GetPosition()))
	{
		_pUI->SetState(make_shared<IdleUIState>());
		_pUI->SetStateType(UIStateType::ST_IDLE);
	}
	else
	{
		_pUI->SetState(make_shared<HoverUIState>());
		_pUI->SetStateType(UIStateType::ST_HOVER);
	}
}