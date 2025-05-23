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
	m_vLook = GetOwner()->GetPosition() - GetOwner()->GetCameraComponent()->GetLocalPosition();
	m_pAnimInstance = GetOwner()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();

	//auto shape = make_shared<UBoxComponent>();
	//shape->SetLocalScale({ 1.f, 2.0f, 1.f });
	//shape->SetLocalRotation({ 0.0f, 0.0f, 0.0f });
	//shape->SetLocalPosition({ 0.0f, 1.0f, 0.0f });
	//shape->SetOwner(GetOwner());
	//shape->Init();
	//GetOwner()->SetShapeComponent(shape);
	
	// Set Socket
	// [����] crow_final6.mesh.json ���� �ÿ��� ����
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

void PlayerMoveScript::Tick()
{
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
		m_bAttack  = true;
		int targetIndex = m_pAnimInstance->GetAnimIndex(L"Slash_Light_R_new");
		m_pAnimInstance->PlayOnce(targetIndex);
	}
	else
	{
		m_bAttack = false;
	}

	if (moveDir.Length() > 0 && !m_pAnimInstance->m_bOnPlayOnce)
	{
		// �ִ� ( ���� ó�� ���� ������Ʈ �ʿ� )
		{
			int targetIndex = m_pAnimInstance->GetAnimIndex(L"Run");
			m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
		}

		// �̵�
		{
			moveDir.Normalize();
			Vec3 pos = moveDir * m_fSpeed * deltaTime;
			GetOwner()->AddPosition(pos);
		}

		// ȸ��		
		{
			float targetYaw = atan2f(moveDir.x, moveDir.z);
			Vec3 currentRot = GetOwner()->GetRotation();
			float currentYaw = currentRot.y;

			// ���� ���� ���
			float angleDiff = targetYaw - currentYaw;
			while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
			while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

			// Lerp ���
			float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * deltaTime;

			currentRot.y = smoothedYaw;
			GetOwner()->SetRotation(currentRot);
		}
	}
	else
	{
		int targetIndex = m_pAnimInstance->GetAnimIndex(L"Idle_0");
		m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
	}

}
