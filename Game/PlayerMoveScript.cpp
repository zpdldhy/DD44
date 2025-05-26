#include "pch.h"
#include "PlayerMoveScript.h"
#include "Input.h"
#include "Timer.h"
#include "APawn.h"
#include "AActor.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "UBoxComponent.h"
#include "MeshTransform.h"

void PlayerMoveScript::Init()
{
	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookAt(GetOwner()->GetPosition());

	m_vLook = GetOwner()->GetPosition() - GetOwner()->GetCameraComponent()->GetLocalPosition();
	m_pAnimInstance = GetOwner()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
}

	//auto shape = make_shared<UBoxComponent>();
	//shape->SetLocalScale({ 1.f, 2.0f, 1.f });
	//shape->SetLocalRotation({ 0.0f, 0.0f, 0.0f });
	//shape->SetLocalPosition({ 0.0f, 1.0f, 0.0f });
	//shape->SetOwner(GetOwner());
	//shape->Init();
	//GetOwner()->SetShapeComponent(shape);

#pragma region crow_final6 Socket
	{
		// Set Socket
		// [주의] crow_final6.mesh.json 파일 시에만 가능
		//sword = GetOwner()->GetMeshComponent()->GetChild(2);
		//effect = GetOwner()->GetMeshComponent()->GetChild(6);
		//sword_socket1 = GetOwner()->GetMeshComponent()->GetChild(3);
		//sword_socket2 = GetOwner()->GetMeshComponent()->GetChild(4);
		//back_socket = GetOwner()->GetMeshComponent()->GetChild(5);

		//effect.lock()->SetVisible(false);
		//sword_socket1.lock()->SetVisible(false);
		//sword_socket2.lock()->SetVisible(false);
		//back_socket.lock()->SetVisible(false);

		//sword.lock()->AddChild(sword_socket1.lock());
		//sword.lock()->AddChild(sword_socket2.lock());


		//auto socketTemp = UStaticMeshComponent::CreateCube();
		//auto transform = make_shared<MeshTransform>();
		////socketTemp->SetMeshTransform(transform);
		//back_socket.lock()->AddChild(sword.lock());

		//sword.lock()->SetLocalPosition(back_socket.lock()->GetAnimWorld());

	}
#pragma endregion

#pragma region TestYR Socket
	{
		sword = GetOwner()->GetMeshComponent()->GetChildByName(L"Sword");
		handSocket = GetOwner()->GetMeshComponent()->GetChildByName(L"HandSocket");
		backSocket = GetOwner()->GetMeshComponent()->GetChildByName(L"BackSocket");
	}
#pragma endregion


	idle = make_shared<PlayerIdleState>(m_pOwner);
	walk = make_shared<PlayerWalkState>(m_pOwner);
	attack = make_shared<PlayerAttackState>(m_pOwner);
	dynamic_pointer_cast<PlayerAttackState>(attack)->SetComponent(sword, handSocket, backSocket);

	currentState = idle;
	currentState->Enter();
}

void PlayerMoveScript::Tick()
{
	currentState->Tick();
	//sword.lock()->SetLocalPosition(back_socket.lock()->GetAnimWorld());
	float deltaTime = TIMER->GetDeltaTime();

	Vec3 up = { 0, 1, 0 };
	m_vRight = up.Cross(m_vLook);
	m_vLook.y = 0.0f;
	m_vRight.y = 0.0f;

	m_vLook.Normalize();
	m_vRight.Normalize();

	Vec3 moveDir;
	if (INPUT->GetButtonDown(UP))
	{
		moveDir += m_vLook;
	}

	if (INPUT->GetButtonDown(LEFT))
	{
		moveDir += -m_vRight;
	}

	if (INPUT->GetButtonDown(DOWN))
	{
		moveDir += -m_vLook;
	}

	if (INPUT->GetButtonDown(RIGHT))
	{
		moveDir += m_vRight;
	}

	if (INPUT->GetButton(LCLICK))
	{
		m_bAttack = true;
		//int targetIndex = m_pAnimInstance->GetAnimIndex(L"Slash_Light_R_new");
		//m_pAnimInstance->PlayOnce(targetIndex);
		ChangetState(attack);

	}
	else
	{
		m_bAttack = false;
	}

	if (moveDir.Length() > 0 && !m_pAnimInstance->m_bOnPlayOnce)
	{
		// 애님 ( 추후 처리 로직 업데이트 필요 )
		{
			//int targetIndex = m_pAnimInstance->GetAnimIndex(L"Run");
			//m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
			ChangetState(walk);
		}

		// 이동
		{
			moveDir.Normalize();
			Vec3 pos = moveDir * m_fSpeed * deltaTime;
			GetOwner()->AddPosition(pos);
		}

		// 회전		
		{
			float targetYaw = atan2f(moveDir.x, moveDir.z);
			Vec3 currentRot = GetOwner()->GetRotation();
			float currentYaw = currentRot.y;

			// 각도 차이 계산
			float angleDiff = targetYaw - currentYaw;
			while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
			while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

			// Lerp 계산
			float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * deltaTime;

			currentRot.y = smoothedYaw;
			GetOwner()->SetRotation(currentRot);
		}
	}
	else
	{
		//int targetIndex = m_pAnimInstance->GetAnimIndex(L"Idle_0");
		//m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
		ChangetState(idle);
	}

	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookAt(GetOwner()->GetPosition());
}

void PlayerMoveScript::ChangetState(shared_ptr<StateBase> _state)
{
	if (!currentState->IsInterruptible() && currentState->IsPlaying())
	{
		return;
	}

	if (currentState)
		currentState->End();

	if (_state->GetId() == PLAYER_STATE::PLAYER_S_ATTACK)
	{
		dynamic_pointer_cast<PlayerAttackState>(_state)->SetPrevState(currentState);
		dynamic_pointer_cast<PlayerAttackState>(_state)->SetCurrentState(&currentState);
	}


	currentState = _state;

	if (currentState)
		currentState->Enter();
}
