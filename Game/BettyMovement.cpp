#include "pch.h"
#include "BettyMovement.h"
#include "Input.h"
#include "EffectManager.h"
//#include "CameraManager.h"

void BettyMovement::Init()
{
	// state
	idle = make_shared<BettyIdleState>(m_pOwner);
	intro = make_shared<BettyIntroState>(m_pOwner);
	tempSlam = make_shared<BettySlamState>(m_pOwner);
	currentState = idle;
	currentState->Enter();
}

void BettyMovement::Tick()
{
	currentState->Tick();

	Vec3 diff = player.lock()->GetPosition() - GetOwner()->GetPosition();
	if (m_bPlayOnce && diff.Length() < 20.0f)
	{
		ChangetState(tempSlam);
		m_bPlayOnce = false;
	}
	if (!m_bPlayOnce && diff.Length() > 20.0f)
	{
		m_bPlayOnce = true;
	}

	if (currentState->GetId() == BETTY_S_GROUNDSLAM && !currentState->IsPlaying())
	{
		currentState->End();
		ChangetState(idle);
	}

	if (INPUT->GetButton(K))
	{
		Vec3 pos = GetOwner()->GetPosition();
		EFFECT->PlayDustBurst(pos, 10.f, .5f);
		EFFECT->PlayEffect(EEffectType::Shockwave, pos, 0.f, Vec3::Zero, .5f);
		EFFECT->PlayBeamBurst(pos, 20, .5f);
	}

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
