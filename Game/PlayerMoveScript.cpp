#include "pch.h"
#include "PlayerMoveScript.h"
#include "Input.h"
#include "Timer.h"
#include "APawn.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "UBoxComponent.h"
#include "PrefabToActor.h"
#include "UIManager.h"
#include "AUIActor.h"

void PlayerMoveScript::Init()
{
	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookAt(GetOwner()->GetPosition());

	m_vLook = GetOwner()->GetPosition() - GetOwner()->GetCameraComponent()->GetLocalPosition();
	m_pAnimInstance = GetOwner()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	
	//SetUI();
}

void PlayerMoveScript::Tick()
{
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
		// 애님 ( 추후 처리 로직 업데이트 필요 )
		{
			int targetIndex = m_pAnimInstance->GetAnimIndex(L"Run");
			m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
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
		int targetIndex = m_pAnimInstance->GetAnimIndex(L"Idle_0");
		m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
	}

	// Update UI State
	//UpdateHPUI();
	UpdateArrowUI();

	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookAt(GetOwner()->GetPosition());
}

void PlayerMoveScript::SetUI()
{
	m_vHPUI.push_back(PToA->MakeUI("../Resources/Prefab/UI_Health_1.ui.json"));
	m_vHPUI.push_back(PToA->MakeUI("../Resources/Prefab/UI_Health_2.ui.json"));
	m_vHPUI.push_back(PToA->MakeUI("../Resources/Prefab/UI_Health_3.ui.json"));
	m_vHPUI.push_back(PToA->MakeUI("../Resources/Prefab/UI_Health_4.ui.json"));

	UI->AddUIList(m_vHPUI);
}

void PlayerMoveScript::UpdateHPUI()
{
	// test용
	if (INPUT->GetButton(K))
	{
		m_vHP--;
		m_bDamaged = true;
	}

	if (INPUT->GetButton(L))
		m_vHP++;

	Color RestColor;

	if (m_vHP == 4)
	{
		RestColor = fullHP;
		RestColor.w = -0.3f;

		m_vHPUI[0]->SetColor(RestColor);
		m_vHPUI[1]->SetColor(RestColor);
		m_vHPUI[2]->SetColor(RestColor);
		m_vHPUI[3]->SetColor(fullHP);

		m_vHPUI[0]->m_bRender = true;
		m_vHPUI[1]->m_bRender = true;
		m_vHPUI[2]->m_bRender = true;
		m_vHPUI[3]->m_bRender = true;
	}

	// 데미지를 입었을 시, UI Animation
	if (m_bDamaged) 
	{
		static float currentTime = 0.0f;
		static float damageTime = 0.0f;

		damageTime = TIMER->GetDeltaTime() + currentTime;

		if (m_vHP == 3)
		{
			m_vHPUI[2]->AddColor(Color(0.f, 0.f, 0.f, currentTime * 1000.f));
			m_vHPUI[3]->m_bRender = false;
		}
		else if (m_vHP == 2)
		{
			m_vHPUI[1]->AddColor(Color(0.f, 0.f, 0.f, currentTime * 1000.f));
			m_vHPUI[2]->m_bRender = false;
		}
		else if (m_vHP == 1)
		{
			RestColor = fullHP;
			RestColor.w = -0.3f;

			m_vHPUI[0]->SetColor(fullHP);

			m_vHPUI[0]->m_bRender = true;
			m_vHPUI[1]->m_bRender = false;
			m_vHPUI[2]->m_bRender = false;
			m_vHPUI[3]->m_bRender = false;
		}
		else if (m_vHP == 0)
		{
			m_vHPUI[0]->m_bRender = false;
			m_vHPUI[1]->m_bRender = false;
			m_vHPUI[2]->m_bRender = false;
			m_vHPUI[3]->m_bRender = false;
		}

		if (currentTime > m_fDamageTime)
		{
			m_bDamaged = false;
			currentTime = 0.0f;
		}
	}

	if (m_vHP > 4)
		m_vHP = 4;
}

void PlayerMoveScript::UpdateArrowUI()
{

	if (m_vArrowCount > 4)
		m_vArrowCount = 4;
}
