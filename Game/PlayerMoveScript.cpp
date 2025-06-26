
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
#include "MeshLoader.h"

shared_ptr<UScriptComponent> PlayerMoveScript::Clone()
{
	auto script = make_shared<PlayerMoveScript>();
	return script;
}

void PlayerMoveScript::Init()
{
	m_fCurrentSpeed = m_fSpeed;

	m_vLook = -m_vCameraOffset;

	empty = make_shared<PlayerEmptyState>();
	idle = make_shared<PlayerIdleState>(m_pOwner);
	walk = make_shared<PlayerWalkState>(m_pOwner);
	climb = make_shared<PlayerClimbState>(m_pOwner);
	roll = make_shared<PlayerRollState>(m_pOwner);
	attack = make_shared<PlayerAttackState>(m_pOwner);
	hit = make_shared<PlayerHitState>(m_pOwner);
	shoot = make_shared<PlayerShootState>(m_pOwner);
	die = make_shared<PlayerDieState>(m_pOwner);

	currentState = idle;
	currentState->Enter();
	currentStateId = PLAYER_S_IDLE;

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
	m_pNoiesTexture = TEXTURE->Get(L"../Resources/Texture/Noise.png");

	// Bow
	bow = make_shared<AActor>();
	auto meshComp = MESHLOADER->Make("../Resources/Asset/bow.mesh.json");
	bow->SetMeshComponent(meshComp);
	OBJECT->AddActor(bow);
	bow->m_bRender = false;
}

void PlayerMoveScript::Tick()
{
	auto player = dynamic_pointer_cast<TPlayer>(GetOwner());
	auto playerPos = player->GetPosition();


	// ��� state ����
	PlayFX();
	CheckCoolTIme();


	currentState->Tick();
	bool currentStateEnd = !currentState->IsPlaying();

	// state�� �߰� ó��
	switch (currentStateId)
	{
	case EMPTY_STATE:
		CheckCollision();
		CheckRoll();
		CheckMove();
		CheckClimb();
		CheckAttack();
		break;
	case PLAYER_S_IDLE:
		CheckCollision();
		CheckRoll();
		CheckMove();
		CheckClimb();
		CheckAttack();
		break;
	case PLAYER_S_WALK:
		CheckCollision();
		CheckRoll();
		CheckMove();
		CheckClimb();
		CheckAttack();
		break;
	case PLAYER_S_ATTACK:
		CheckComboAttack();
		CheckCollision();
		if (currentStateEnd)
		{
			attackRangeActor->m_bCollision = false;
			attackRangeActor->GetShapeComponent()->m_bVisible = false;
			ChangeState(empty);
		}
		break;
	case PLAYER_S_CLIMB:
		if (currentStateEnd)
		{
			player->StopClimbing();
			m_bCanClimb = false;
		}
		if (playerPos.y > m_vLadderEnd.y)
		{
			if (currentState == climb)
			{
				dynamic_pointer_cast<PlayerClimbState>(currentState)->CheckClimbFinish(true);
			}
		}
		// �ڿ������� ó�� ����
		CheckCollision();
		CheckMove();
		break;
	case PLAYER_S_SHOOT:
		CheckCollision();
		CheckRoll();
		UpdateBow();
		if (INPUT->GetButtonUp(RCLICK) || INPUT->GetButtonFree(RCLICK))
		{

			// ������ ��ȣ �ֱ�
			if (currentState == shoot)
			{
				dynamic_pointer_cast<PlayerShootState>(currentState)->CheckEnd(true);
			}
		}
		else
		{
			return;
		}

		if (currentStateEnd)
		{
			bow->m_bRender = false;
			ChangeState(empty);
		}
		break;
	case PLAYER_S_DEATH:
		return;
		break;
	case PLAYER_S_HIT:
		CheckCollision();
		if (currentStateEnd)
		{
			m_bDamageCoolTime = true;
			ChangeState(empty);
		}
		break;
	case PLAYER_S_ROLL:
		if (currentStateEnd)
		{
			m_bRollCoolTime = true;
			ChangeState(idle);
		}
		else
		{
			RollMove();
			return;
		}
		break;
	default:
		break;
	}

	// Trigger Ȯ��
	player->ClearTrigger();
	if (GetOwner()->m_vCollisionList.size() > 0)
	{
		for (auto& col : GetOwner()->m_vCollisionList)
		{
			auto pObj = OBJECT->GetActor(col.first);
			if (pObj->GetShapeComponent()->GetName() == L"Ladder")
			{
				TriggerData data;
				data.eTriggerType = ETriggerType::TT_LADDER;

				if (pObj->GetRotation().y > 0.1f || pObj->GetRotation().y < -0.1f)
					data.vPoint = pObj->GetPosition() + Vec3(-1.5f, 2.f, -1.5f);	// UI ��ġ ������, ���� ����.
				else
					data.vPoint = pObj->GetPosition() + Vec3(1.5f, 2.f, 1.5f);		// UI ��ġ ������, ���� ����.

				player->SetTrigger(data);
			}

			if (pObj->GetShapeComponent()->GetName() == L"Healer")
			{
				TriggerData data;
				data.eTriggerType = ETriggerType::TT_HEALPOINT;
				data.vPoint = pObj->GetPosition() + Vec3(2.f);	// UI ��ġ ������, ���� ����.
				player->SetTrigger(data);
			}
		}
	}

#pragma region TEMP
	// Test
	if (INPUT->GetButton(L))
	{
		Resurrection();
	}
#pragma endregion

}

void PlayerMoveScript::Resurrection()
{
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(dynamic_pointer_cast<TCharacter>(GetOwner())->GetMaxHp());
	if (currentState->GetId() == PLAYER_S_DEATH)
	{
		currentState->End();
		ChangeState(idle);
	}
}

void PlayerMoveScript::ChangeState(shared_ptr<StateBase> _state)
{
	if (_state == currentState)
	{
		return;
	}

	if (_state->GetId() == PLAYER_S_HIT)
	{
		if (currentStateId == PLAYER_S_ROLL)
		{
			m_bCanRoll = true;
		}
	}
	
	if (currentStateId == PLAYER_S_SHOOT)
	{
		bow->m_bRender = false;
	}
	else if (!currentState->IsInterruptible() && currentState->IsPlaying())
	{
		return;
	}

	if (currentState)
		currentState->End();

	currentState = _state;

	if (currentState)
	{
		currentState->Enter();
		currentStateId = (PLAYER_STATE)currentState->GetId();
	}
}

void PlayerMoveScript::CheckCollision()
{
	// ����ü �浹 Ȯ��
	auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());

	// �浹 Ȯ��
	if (m_bCanBeHit)
	{
		// ���� ���� Ȯ��
		bool isCol = false;
		if (GetOwner()->m_vCollisionList.size() > 0)
		{
			auto list = GetOwner()->m_vCollisionList;
			for (auto& index : list)
			{
				if (OBJECT->GetActor(index.first)->m_szName == L"Enemy")
				{
					isCol = true;
				}
				// CanBeHit �Ʒ��� �־ �Ǵ� ���� Ȯ�� �ʿ�
				// ladder ó�� �ǹ�
				auto actor = OBJECT->GetActor(index.first);
				auto shape = actor->GetShapeComponent();
				if (shape->GetName() == L"Ladder")
				{
					auto ladderTrigger = OBJECT->GetActor(index.first);
					m_vLadder = ladderTrigger->GetPosition();
					m_vLadderEnd = ladderTrigger->GetMeshComponent()->GetChildByName(L"ladderEnd")->GetWorldPosition();
					auto firstLadder = ladderTrigger->GetMeshComponent()->GetChildByName(L"ladder1");
					m_vDirToLadder = firstLadder->GetWorldPosition() - m_vLadder;
					m_vDirToLadder.Normalize();
					m_bCanClimb = true;
				}
			}
		}

		if (isCol || healthComp->IsHitByProjectile())
		{
			{
				// Blood FX
				Vec3 basePos = GetOwner()->GetPosition();
				basePos.y += RandomRange(0.5, 2);
				Vec3 look = GetOwner()->GetLook();
				velocity = -look;
				PlayBloodBurst(basePos, velocity, 50.0f, 90.0f);

				m_fHitFlashTimer = 1.f;  // 1�� ����
				m_bIsFlashing = true;

				// Anim - HP 
				dynamic_pointer_cast<TCharacter>(GetOwner())->TakeDamage(1);

				if (!dynamic_pointer_cast<TCharacter>(GetOwner())->IsDead())
				{
					ChangeState(hit);
					m_bCanBeHit = false;
				}
				else
				{
					ChangeState(die);
				}
			}
		}
	}
}

void PlayerMoveScript::PlayFX()
{
	DC->PSSetShaderResources(1, 1, m_pSubTexture->GetSRV().GetAddressOf());
	DC->PSSetShaderResources(2, 1, m_pNoiesTexture->GetSRV().GetAddressOf());
	ApplyCrash();

	//Slash();
	if (m_bIsFlashing)
	{
		m_fHitFlashTimer -= TIMER->GetDeltaTime();
		if (m_fHitFlashTimer <= 0.0f)
		{
			m_fHitFlashTimer = 0.0f;
			m_bIsFlashing = false;
		}

		// hitFlashAmount�� 1 �� 0 ���� ����
		float hitFlashAmount = std::min(std::max<float>(m_fHitFlashTimer, 0.0f), 1.0f);

		auto root = GetOwner()->GetMeshComponent();
		ApplyHitFlashToAllMaterials(root, hitFlashAmount);
	}
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
// �� �Լ�, ��� �ʿ��ϸ� �ֱ��.
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
		mat->SetHitFlashTime(value); // CB�� g_fHitFlashTime ����
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
		// �ִ�
		{
			ChangeState(walk);
		}

		// �̵�
		{
			moveDir.Normalize();
			//Vec3 pos = moveDir * m_fCurrentSpeed * deltaTime;
			auto maxSpeed = dynamic_pointer_cast<TPlayer>(GetOwner())->GetMoveSpeed();
			GetOwner()->SetMove(moveDir, maxSpeed);
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

		m_vLastMoveDir = moveDir;

	}
	else
	{
		ChangeState(idle);
	}
}
void PlayerMoveScript::Climb()
{
	if (currentState != climb)
	{
		return;
	}

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
		moveDir += up;
	}

	if (INPUT->GetButtonDown(S))
	{
		moveDir += -up;
	}
	if (moveDir.Length() > 0)
	{
		auto fin = dynamic_pointer_cast<PlayerClimbState>(currentState)->GetCurrentPhase();
		if (fin)
		{
			return;
		}
		// �̵�
		{
			// Anim control �� state�� �־������. flag ������ ���ִ� ������� ���޸� �� 
			dynamic_pointer_cast<PlayerClimbState>(currentState)->CheckIsMoving(true);
			Vec3 pos = moveDir * 0.2f;
			GetOwner()->AddPosition(pos);

		}
	}
	else
	{
		dynamic_pointer_cast<PlayerClimbState>(currentState)->CheckIsMoving(false);
	}
}

void PlayerMoveScript::RollMove()
{
	//Vec3 pos = m_vRollLook * m_fRollSpeed * TIMER->GetDeltaTime();
	auto maxRollSpeed = dynamic_pointer_cast<TPlayer>(GetOwner())->GetRollSpeed();
	GetOwner()->SetMove(m_vRollLook, maxRollSpeed);
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

void PlayerMoveScript::UpdateBow()
{
	Vec3 bowPos = GetOwner()->GetMeshComponent()->GetChildByName(L"LeftHandSocket")->GetWorldPosition();
	bow->SetPosition(bowPos);
	float targetYaw = atan2f(GetOwner()->GetLook().x, GetOwner()->GetLook().z);
	Vec3 currentRot = GetOwner()->GetRotation();
	currentRot.y = targetYaw;
	bow->SetRotation(currentRot);
}

void PlayerMoveScript::CheckCoolTIme()
{
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
}

void PlayerMoveScript::CheckClimb()
{
	if (m_bCanClimb && INPUT->GetButton(E))
	{
		GetOwner()->SetPosition(m_vLadder);
		dynamic_pointer_cast<TPlayer>(GetOwner())->StartClimbing();
		dynamic_pointer_cast<PlayerClimbState>(climb)->SetLadderDir(m_vDirToLadder);
		ChangeState(climb);
	}
}

void PlayerMoveScript::CheckRoll()
{
	if (m_bNoInput)
		return;

	if (INPUT->GetButton(SPACE) && m_bCanRoll)
	{
		// ������
		m_vRollLook = GetOwner()->GetLook();
		ChangeState(roll);

		m_bCanRoll = false;
	}
}

void PlayerMoveScript::CheckAttack()
{
	if (m_bNoInput)
		return;

	// ATTACK
	if (INPUT->GetButton(LCLICK))
	{
		if (currentState->GetId() != PLAYER_S_ATTACK)
		{
			ChangeState(attack);

			UpdateCollider();

			attackRangeActor->m_bCollision = true;
			attackRangeActor->GetShapeComponent()->m_bVisible = true;
		}
	}
	if (INPUT->GetButton(RCLICK))
	{
		UpdateBow();
		bow->m_bRender = true;
		// TPlayer�� arrowCount Ȯ���ؼ� bool �����ϱ� 
		int aCount = dynamic_pointer_cast<TPlayer>(GetOwner())->GetArrowCount();
		if (aCount > 0)
		{
			dynamic_pointer_cast<PlayerShootState>(shoot)->CheckShootCount(true);
		}
		else
		{
			dynamic_pointer_cast<PlayerShootState>(shoot)->CheckShootCount(false);
		}
		ChangeState(shoot);
	}
}

void PlayerMoveScript::CheckComboAttack()
{
	// �Է� Ȯ���ϰ�
	if (INPUT->GetButton(LCLICK))
	{
		dynamic_pointer_cast<PlayerAttackState>(currentState)->CheckAttackCombo(true);
	}
}

void PlayerMoveScript::CheckMove()
{
	if (m_bNoInput)
		return;

	auto player = dynamic_pointer_cast<TPlayer>(GetOwner());
	if (player->IsClimbing())
	{
		Climb();
	}
	else
	{
		Move();

	}
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
void PlayerMoveScript::ApplyDissolveToAllMaterials(shared_ptr<UMeshComponent> comp, float _time)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetDissolve(_time);
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyDissolveToAllMaterials(comp->GetChild(i), _time);
	}
}
void PlayerMoveScript::ApplyCrash()
{
	auto root = GetOwner()->GetMeshComponent();
	auto hp = dynamic_pointer_cast<TCharacter>(GetOwner())->GetHp();
	if (hp < 3 && !m_bCrashSet)
	{
		m_bCrashSet = true;
	}
	else
	{
		m_bCrashSet = false;
	}
	ApplyCrashToAllMaterials(root, m_bCrashSet);
}