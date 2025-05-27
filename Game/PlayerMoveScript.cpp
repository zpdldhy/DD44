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
#include "PrefabToActor.h"
#include "UIManager.h"
#include "AUIActor.h"
#include "EffectManager.h"


void PlayerMoveScript::Init()
{
	m_vLook = -m_vCameraOffset;
	m_pAnimInstance = GetOwner()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();

	idle = make_shared<PlayerIdleState>(m_pOwner);
	walk = make_shared<PlayerWalkState>(m_pOwner);
	attack = make_shared<PlayerAttackState>(m_pOwner);
	dynamic_pointer_cast<PlayerAttackState>(attack)->SetComponent(sword.lock(), handSocket.lock(), backSocket.lock());

	//SetUI();

	currentState = idle;
	currentState->Enter();

	m_pSlashMaterial = GetOwner()->GetMeshComponent()->GetMeshByName(L"Slash")->GetMaterial();
}

void PlayerMoveScript::Tick()
{
	currentState->Tick();

	float deltaTime = TIMER->GetDeltaTime();
#pragma region �̵��� ���
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

#pragma endregion

	if (INPUT->GetButton(LCLICK))
	{
		m_bAttack = true;
		//int targetIndex = m_pAnimInstance->GetAnimIndex(L"Slash_Light_R_new");
		//m_pAnimInstance->PlayOnce(targetIndex);
		//ChangetState(attack);
		m_bSlashPlaying = true;
		m_fSlashTime = 0.0f;
	}
	else
	{
		m_bAttack = false;
	}

	if (INPUT->GetButton(J))
	{
		Vec3 basePos = GetOwner()->GetPosition();
		Vec3 look = GetOwner()->GetLook();
		Vec3 velocity = look * -1.0f;
		//basePos.z += velocity.z * 0.8f;
		//basePos.y += 0.2f;
		PlayBloodBurst(basePos, velocity, 10.0f, 90.0f);
	}

	// �ǽð� look�� Ȯ���Ϸ���, �Ʒ� �ּ��� Ǯ�� ���â�� ��ϴ� (����.. )
	//Vec3 look = GetOwner()->GetLook();
	//Profiler p(to_string(look.x) + " " + to_string(look.z));


	Slash();

#pragma region �̵� �� ȸ��
	if (moveDir.Length() > 0 && !m_pAnimInstance->m_bOnPlayOnce)
	{
		// �ִ� ( ���� ó�� ���� ������Ʈ �ʿ� )
		{
			//int targetIndex = m_pAnimInstance->GetAnimIndex(L"Run");
			//m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
			ChangetState(walk);
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
		//int targetIndex = m_pAnimInstance->GetAnimIndex(L"Idle_0");
		//m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
		ChangetState(idle);
	}
#pragma endregion

	// Update UI State
	//UpdateHPUI();
	UpdateArrowUI();
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

void PlayerMoveScript::Slash()
{
	if (m_bSlashPlaying)
	{
		m_fSlashTime += TIMER->GetDeltaTime();

		float t = m_fSlashTime;
		float progress = 0.0f;


		if (t <= 0.3f)
		{
			float ratio = t / 0.3f;
			progress = pow(ratio, 2.0f);
		}
		else
		{
			progress = -1.0f;
		}

		if (m_pSlashMaterial)
			m_pSlashMaterial->SetSlashProgress(progress);

		if (t >= m_fSlashDuration)
		{
			m_bSlashPlaying = false;
		}
	}
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
	// test��
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

	// �������� �Ծ��� ��, UI Animation
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

void PlayerMoveScript::PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount, int _maxCount)
{
	int count = RandomRange(_minCount, _maxCount);
	for (int i = 0; i < count; ++i)
	{
		Vec3 offset = Vec3(RandomRange(-0.3f, 0.3f), RandomRange(-0.3f, 0.3f), RandomRange(-0.3f, 0.3f));
		Vec3 pos = _origin + offset;

		Vec3 baseVelocity = _direction * _speed;
		EFFECT->PlayEffect(EEffectType::Blood, pos, _spreadAngleDeg, baseVelocity);
	}
}
