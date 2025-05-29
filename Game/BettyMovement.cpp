#include "pch.h"
#include "BettyMovement.h"
//#include "CameraManager.h"

void BettyMovement::Init()
{
	// state
	idle = make_shared<BettyIdleState>(m_pOwner);
	intro = make_shared<BettyIntroState>(m_pOwner);
	currentState = idle;
	currentState->Enter();
}

void BettyMovement::Tick()
{
	//currentState->Tick();

	//// 1회성 Intro 재생
	//Vec3 diff = player.lock()->GetPosition() - GetOwner()->GetPosition();
	//if (b)
	//{
	//	if (diff.Length() < 50.0f)
	//	{
	//		CAMERA->ChangeCamera(1);
	//		ChangetState(intro);
	//		b = false;
	//	}
	//}

	//if (currentState->GetId() == BETTY_S_INTRO && !currentState->IsPlaying())
	//{
	//	CAMERA->ChangeCamera(2);
	//}


}

void BettyMovement::ChangetState(shared_ptr<StateBase> _state)
{

	if (!currentState->IsInterruptible() && currentState->IsPlaying())
	{
		//if (!_state->GetId() == ENEMY_S_DEATH)
		{
			return;
		}
	}

	if (currentState)
		currentState->End();

	currentState = _state;

	if (currentState)
		currentState->Enter();
}
