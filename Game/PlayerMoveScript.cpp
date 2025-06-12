#include "pch.h"
#include "PlayerMoveScript.h"
#include "Input.h"
#include "Timer.h"
#include "TPlayer.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "UBoxComponent.h"
#include "PrefabToActor.h"
#include "UIManager.h"
#include "AUIActor.h"
#include "EffectManager.h"
#include "UPhysicsComponent.h"
#include "ObjectManager.h"
#include "EnemyCollisionManager.h"

#include "ProjectileManager.h"
#include "Texture.h"

shared_ptr<UScriptComponent> PlayerMoveScript::Clone()
{
	auto script = make_shared<PlayerMoveScript>();
	return script;
}

void PlayerMoveScript::Init()
{
	m_fCurrentSpeed = m_fSpeed;

	m_vLook = -m_vCameraOffset;

	idle = make_shared<PlayerIdleState>(m_pOwner);
	walk = make_shared<PlayerWalkState>(m_pOwner);
	roll = make_shared<PlayerRollState>(m_pOwner);
	attack = make_shared<PlayerAttackState>(m_pOwner);
	hit = make_shared<PlayerHitState>(m_pOwner);
	shoot = make_shared<PlayerShootState>(m_pOwner);
	die = make_shared<PlayerDieState>(m_pOwner);

	SetUI();

	currentState = idle;
	currentState->Enter();

	auto body = dynamic_pointer_cast<UMeshComponent>(GetOwner()->GetMeshComponent());

	m_pSlashMaterial = GetOwner()->GetMeshComponent()->GetChildByName(L"Slash")->GetMaterial();

	//body->GetChildByName(L"Body");
	backSword = body->GetChildByName(L"Sword");
	handSword = body->GetChildByName(L"Sword2");
	// SetPhysics
	auto pPhysics = GetOwner()->GetPhysicsComponent();
	pPhysics->SetWeight(1.f);

	// AttackRange
	attackRangeActor = make_shared<AActor>();
	attackRangeActor->m_bCollision = false;

	auto collider = make_shared<UBoxComponent>();
	collider->m_bVisible = true;
	collider->SetName(L"Melee");

	collider->SetLocalScale(Vec3(3.0f, 1.0f, 2.0f));
	float targetYaw = atan2f(GetOwner()->GetLook().x, GetOwner()->GetLook().z);
	Vec3 currentRot = GetOwner()->GetRotation();
	currentRot.y = targetYaw;
	attackRangeActor->SetRotation(currentRot);

	//collider->SetLocalRotation()
	collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	attackRangeActor->SetShapeComponent(collider);

	colOffset = Vec3(3.0f, 0.0f, 3.0f);
	attackRangeActor->SetPosition(GetOwner()->GetPosition() + colOffset * GetOwner()->GetLook());
	attackRangeActor->m_szName = L"Melee";

	OBJECT->AddActor(attackRangeActor);
	ENEMYCOLLIDER->Add(attackRangeActor);
	collider->m_bVisible = false;

	// 
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(4);

	// Texture
	m_pSubTexture = TEXTURE->Get(L"../Resources/Texture/cracks_generic 1.png");

	{
		auto root = GetOwner()->GetMeshComponent();
		ApplyCrashToAllMaterials(root, false);
	}
}

void PlayerMoveScript::Tick()
{
	UpdateCollider();

	// Test
	if (INPUT->GetButton(L))
	{
		dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(4);
		if (currentState->GetId() == PLAYER_S_DEATH)
		{
			currentState->End();
			ChangetState(idle);
		}
	}

	DC->PSSetShaderResources(1, 1, m_pSubTexture->GetSRV().GetAddressOf());
	ApplyCrash();

#pragma region FX
	Slash();
	if (m_bIsFlashing)
	{
		m_fHitFlashTimer -= TIMER->GetDeltaTime();
		if (m_fHitFlashTimer <= 0.0f)
		{
			m_fHitFlashTimer = 0.0f;
			m_bIsFlashing = false;
		}

		// hitFlashAmount는 1 → 0 으로 감소
		float hitFlashAmount = std::min(std::max<float>(m_fHitFlashTimer, 0.0f), 1.0f);

		auto root = GetOwner()->GetMeshComponent();
		ApplyHitFlashToAllMaterials(root, hitFlashAmount);
	}
#pragma endregion

	// UI 
	UpdateHPUI();
	UpdateArrowUI();

#pragma region STATE_ANIM
	if (m_bDamageCoolTime)
	{
		m_fDamageCoolTime -= TIMER->GetDeltaTime();
		if (m_fDamageCoolTime < 0)
		{
			m_bDamageCoolTime = false;
			m_fDamageCoolTime = 1.0f;
			m_bCanBeHit = true;
		}
	}
	if (m_bRollCoolTime)
	{
		m_fRollCoolTime -= TIMER->GetDeltaTime();
		if (m_fRollCoolTime < 0)
		{
			m_bRollCoolTime = false;
			m_fRollCoolTime = 0.5f;
			m_bCanRoll = true;
		}
	}

	// 순서 중요
	if (currentState->GetId() == PLAYER_S_SHOOT)
	{
		if (INPUT->GetButtonUp(RCLICK) || INPUT->GetButtonFree(RCLICK))
		{
			// 끝내라 신호를 줘야함. 
			dynamic_pointer_cast<PlayerShootState>(currentState)->CheckEnd(true);
		}
	}

	// 순서 중요
	CheckHit();

	currentState->Tick();
	if (currentState->GetId() == PLAYER_STATE::PLAYER_S_DEATH)
	{
		return;
	}

	// 이걸 왜 이렇게 했던 거지 ?
	//if (currentState->GetId() == PLAYER_STATE::PLAYER_S_ATTACK || currentState->GetId() == PLAYER_STATE::PLAYER_S_HIT || currentState->GetId() == PLAYER_STATE::PLAYER_S_ROLL)
	{
		if (!currentState->IsPlaying())
		{
			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_ATTACK)
			{
				handSword.lock()->SetVisible(false);
				backSword.lock()->SetVisible(true);

				attackRangeActor->m_bCollision = false;
				attackRangeActor->GetShapeComponent()->m_bVisible = false;
				ChangetState(idle);

			}
			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_HIT)
			{
				m_bDamageCoolTime = true;
				ChangetState(idle);

			}
			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_ROLL)
			{
				m_bRollCoolTime = true;
				ChangetState(idle);

			}
		}
		else
		{
			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_ATTACK)
			{
				return;
			}
			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_ROLL)
			{
				RollMove();
				return;
			}

			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_HIT)
			{
				return;
			}

			if (currentState->GetId() == PLAYER_STATE::PLAYER_S_SHOOT)
			{
				return;
			}
		}
	}
#pragma endregion
	Move();

	if (INPUT->GetButton(SPACE) && m_bCanRoll)
	{
		// 구르기
		m_vRollLook = GetOwner()->GetLook();
		ChangetState(roll);

		m_bCanRoll = false;
	}

	// ATTACK
	if (INPUT->GetButton(LCLICK))
	{
		handSword.lock()->SetVisible(true);
		backSword.lock()->SetVisible(false);
		ChangetState(attack);
		m_bSlashPlaying = true;
		m_fSlashTime = 0.0f;

		//
		attackRangeActor->m_bCollision = true;
		attackRangeActor->GetShapeComponent()->m_bVisible = true;
	}
	if (INPUT->GetButton(RCLICK))
	{
		// TPlayer의 arrowCount 확인해서 bool 세팅하기 
		int aCount = dynamic_pointer_cast<TPlayer>(GetOwner())->GetArrowCount();
		if (aCount > 0)
		{
			dynamic_pointer_cast<PlayerShootState>(shoot)->CheckShootCount(true);
		}
		else
		{
			dynamic_pointer_cast<PlayerShootState>(shoot)->CheckShootCount(false);
		}
		ChangetState(shoot);
	}



}

void PlayerMoveScript::ChangetState(shared_ptr<StateBase> _state)
{
	if (_state == currentState)
	{
		return;
	}

	if (!currentState->IsInterruptible() && currentState->IsPlaying())
	{
		return;
	}

	if (currentState)
		currentState->End();

	currentState = _state;

	if (currentState)
		currentState->Enter();
}

void PlayerMoveScript::CheckHit()
{
	// 투사체 충돌 확인
	auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());

	// 충돌 확인
	if (m_bCanBeHit)
	{
		// 근접 공격 확인
		bool isCol = false;
		if (GetOwner()->m_vCollisionList.size() > 0)
		{
			auto list = GetOwner()->m_vCollisionList;
			for (auto& index : list)
			{
				if (OBJECT->GetActor(index.first)->m_szName == L"Enemy")
					isCol = true;
			}
		}

		if (isCol || healthComp->IsHitByProjectile())
		{
			m_bHPUIChange = true;
			{
				// Blood FX
				Vec3 basePos = GetOwner()->GetPosition();
				basePos.y += RandomRange(0.5, 2);
				Vec3 look = GetOwner()->GetLook();
				velocity = -look;
				PlayBloodBurst(basePos, velocity, 50.0f, 90.0f);

				m_fHitFlashTimer = 1.f;  // 1초 동안
				m_bIsFlashing = true;

				// Anim - HP 
				dynamic_pointer_cast<TCharacter>(GetOwner())->TakeDamage(1);

				if (!dynamic_pointer_cast<TCharacter>(GetOwner())->IsDead())
				{
					ChangetState(hit);
					m_bCanBeHit = false;
				}
				else
				{
					ChangetState(die);
				}
			}
		}
	}
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
	m_vHPUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_HP.uis.json");
	m_vArrowUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_Arrow.uis.json");
	UI->AddUIList(m_vHPUI);
	UI->AddUIList(m_vArrowUI);
}

void PlayerMoveScript::UpdateHPUI()
{
	Color RestColor;
	auto hp = dynamic_pointer_cast<TCharacter>(GetOwner())->GetHp();
	if (hp == 4)
	{
		RestColor = fullHP;
		RestColor.w = -0.5f;

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
	if (m_bHPUIChange)
	{
		static float currentTime = 0.0f;
		static float damageTime = 0.0f;

		damageTime += currentTime = TIMER->GetDeltaTime();

		if (hp == 3)
		{
			if (m_vHPUI[2]->GetColor().w < 0.f)
				m_vHPUI[2]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
			else
				m_bHPUIChange = false;

			m_vHPUI[3]->m_bRender = false;
		}
		else if (hp == 2)
		{
			if (m_vHPUI[1]->GetColor().w < 0.f)
				m_vHPUI[1]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
			else
				m_bHPUIChange = false;

			m_vHPUI[2]->m_bRender = false;
		}
		else if (hp == 1)
		{
			if (m_vHPUI[0]->GetColor().w < 0.f)
				m_vHPUI[0]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
			else
				m_bHPUIChange = false;

			m_vHPUI[1]->m_bRender = false;
		}
		else if (hp == 0)
		{
			m_vHPUI[0]->m_bRender = false;
			m_vHPUI[1]->m_bRender = false;
			m_vHPUI[2]->m_bRender = false;
			m_vHPUI[3]->m_bRender = false;
		}
	}

	if (hp > 4)
		hp = 4;
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
// 빈 함수, 기능 필요하면 넣기용.
void PlayerMoveScript::VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{

	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		VisitAllMeshMaterials(comp->GetChild(i));
	}
}

void PlayerMoveScript::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetHitFlashTime(value); // CB에 g_fHitFlashTime 전달
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyHitFlashToAllMaterials(comp->GetChild(i), value);
	}
}

void PlayerMoveScript::Move()
{
	float deltaTime = TIMER->GetDeltaTime();
	Vec3 up = { 0, 1, 0 };
	m_vRight = up.Cross(m_vLook);
	m_vLook.y = 0.0f;
	m_vRight.y = 0.0f;

	m_vLook.Normalize();
	m_vRight.Normalize();

	Vec3 moveDir;
	if (INPUT->GetButtonDown(W))
	{
		moveDir += m_vLook;
	}

	if (INPUT->GetButtonDown(A))
	{
		moveDir += -m_vRight;
	}

	if (INPUT->GetButtonDown(S))
	{
		moveDir += -m_vLook;
	}

	if (INPUT->GetButtonDown(D))
	{
		moveDir += m_vRight;
	}

	if (moveDir.Length() > 0)// && !m_pAnimInstance->m_bOnPlayOnce)
	{
		// 애님
		{
			ChangetState(walk);
		}

		// 이동
		{
			moveDir.Normalize();
			//Vec3 pos = moveDir * m_fCurrentSpeed * deltaTime;
			GetOwner()->SetMove(moveDir, 0.25f);
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

		m_vLastMoveDir = moveDir;

	}
	else
	{
		ChangetState(idle);
	}
}

void PlayerMoveScript::RollMove()
{
	//Vec3 pos = m_vRollLook * m_fRollSpeed * TIMER->GetDeltaTime();	
	GetOwner()->SetMove(m_vRollLook, 0.5f);
}

bool PlayerMoveScript::CanAttack()
{
	if (currentState->GetId() == PLAYER_S_HIT || currentState->GetId() == PLAYER_S_DEATH)
	{
		return false;
	}
	return true;
}

void PlayerMoveScript::UpdateCollider()
{
	auto pos = GetOwner()->GetPosition();
	attackRangeActor->SetPosition(pos + colOffset * GetOwner()->GetLook() + Vec3(0.0f, 2.0f, 0.0f));
	float targetYaw = atan2f(GetOwner()->GetLook().x, GetOwner()->GetLook().z);
	Vec3 currentRot = GetOwner()->GetRotation();
	currentRot.y = targetYaw;
	attackRangeActor->SetRotation(currentRot);
}

void PlayerMoveScript::ApplyCrashToAllMaterials(shared_ptr<UMeshComponent> comp, bool enabled)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetCrash(enabled);
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyCrashToAllMaterials(comp->GetChild(i), enabled);
	}
}

void PlayerMoveScript::ApplyCrash()
{
	auto root = GetOwner()->GetMeshComponent();
	if (m_vHP < 3 && !m_bCrashSet)
	{
		m_bCrashSet = true;
	}
	else
	{
		m_bCrashSet = false;
	}
	ApplyCrashToAllMaterials(root, m_bCrashSet);
}