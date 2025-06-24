#include "pch.h"
#include "BettyMovement.h"
#include "EffectManager.h"
//#include "CameraManager.h"

#include "Timer.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "TEnemy.h"
#include "TCharacter.h"
#include "UStaticMeshComponent.h"
#include "UBoxComponent.h"
#include "EnemyCollisionManager.h"

// temp temp temp !!!!!
#include "Input.h"

void BettyMovement::Init()
{
	SetPlayer(dynamic_pointer_cast<TEnemy>(GetOwner())->GetPlayer());

	// state
	idle = make_shared<BettyIdleState>(m_pOwner);
	intro = make_shared<BettyIntroState>(m_pOwner);
	jumpAttack = make_shared<BettyJumpAttack>(m_pOwner);
	twoHandAttack = make_shared<BettyTwoHandAttack>(m_pOwner);
	oneHandBackAttack = make_shared<BettyOneHandBackAttack>(m_pOwner);
	oneHandDownAttack = make_shared<BettyOneHandDownAttack>(m_pOwner);
	rollAttack = make_shared<BettyRollAttack>(m_pOwner);
	dropAttack = make_shared<BettyDropAttack>(m_pOwner);
	roarAttack = make_shared<BettyRoarAttack>(m_pOwner);
	death = make_shared<BettyDeathState>(m_pOwner);

	meleeState.push_back(twoHandAttack);
	meleeState.push_back(oneHandDownAttack);
	meleeState.push_back(oneHandBackAttack);

	rangedState.push_back(jumpAttack);
	rangedState.push_back(rollAttack);
	// 3페이즈에 가서 추가
	//rangedState.push_back(dropAttack);

	ChangeState(idle);

	SetSnowBall();

	AddColliderActor();
	
	// HP
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(20);
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetSoul(1000);


}

void BettyMovement::Tick()
{
	leftRange->SetPosition(leftHand.lock()->GetWorldPosition());
	rightRange->SetPosition(rightHand.lock()->GetWorldPosition());

	// 공격
	switch (currentAction)
	{
	case BettyAction::Intro: {
		// 진입 조건 변경 필요 
		Vec3 diff = player.lock()->GetPosition() - GetOwner()->GetPosition();
		if (diff.Length() < 20.0f)
		{
			ChangeState(intro);
			currentAction = BettyAction::Attack;
		}
		break;
	}
	case BettyAction::Attack: {
		auto delta = TIMER->GetDeltaTime();
		currentState->Tick();

		if (currentStateId == BETTY_S_IDLE || !currentState->IsPlaying())
		{
			currentState->End();
			// 거리 확인
			distance = (player.lock()->GetPosition() - GetOwner()->GetPosition());
			GetOwner()->m_szName = L"Betty";
			GetOwner()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
			rightRange->m_bCollision = false;
			rightRange->GetShapeComponent()->m_bVisible = false;
			leftRange->m_bCollision = false;
			leftRange->GetShapeComponent()->m_bVisible = false;
			HandleAttack(delta);
		}
		if (bSnowControl)
		{
			//auto delta = TIMER->GetDeltaTime();
			DropSnowBall(delta);
			HandleSnowBall();
		}
		break;
	}
	case BettyAction::Die: {
		auto delta = TIMER->GetDeltaTime();
		currentState->Tick();
	}
	}

	// 피격
	if (currentState->GetId() != BETTY_STATE::BETTY_S_DEATH)
	{
		//// 피격 확인
		CheckHit();
	}
	//// 피격 효과
	Flashing();

	// 손 위치에 맞게 넣기
	if (INPUT->GetButton(K))
	{
		Vec3 pos = GetOwner()->GetPosition();
		EFFECT->PlayDustBurst(pos, 10.f, .5f);
		EFFECT->PlayEffect(EEffectType::Shockwave, pos, 0.f, Vec3::Zero, .5f);
		EFFECT->PlayBeamBurst(pos, 20, .5f);
		EFFECT->PlayEffect(EEffectType::BloodDecal, pos, 0.f, Vec3::Zero, .5f);
	}

}

shared_ptr<UScriptComponent> BettyMovement::Clone()
{
	auto script = make_shared<BettyMovement>();
	return script;
}

#pragma region Snowball
void BettyMovement::SetSnowBall()
{
	snowList.resize(15);
	for (int i = 0; i < 15; i++)
	{
		snowList[i] = CreateSnowBall();
	}

}
shared_ptr<AActor> BettyMovement::CreateSnowBall()
{
	auto snow = make_shared<AActor>();
	auto mesh = UStaticMeshComponent::CreateSphere(13, 13);
	mesh->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));
	auto material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/white.png", L"../Resources/Shader/Default.hlsl");
	mesh->SetMaterial(material);
	snow->SetMeshComponent(mesh);
	auto collider = make_shared<UBoxComponent>();
	collider->SetName(L"Enemy");
	collider->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));
	collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	snow->SetShapeComponent(collider);
	snow->m_szName = L"Enemy";

	auto pPhysics = GetOwner()->GetPhysicsComponent();
	pPhysics->SetWeight(0.f);

	OBJECT->AddActor(snow);
	ENEMYCOLLIDER->Add(snow);

	snow->m_bCollision = false;
	mesh->SetVisible(false);
	collider->m_bVisible = false;

	return snow;
}
shared_ptr<AActor> BettyMovement::GetSnowBall()
{
	for (auto& snow : snowList)
	{
		if (snow->m_bCollision == false)
		{
			return snow;
		}
	}

	// no more snow
	{
		return CreateSnowBall();
	}

}
void BettyMovement::HandleSnowBall()
{
	for (auto iter = activeSnowList.begin(); iter != activeSnowList.end(); )
	{
		// ground 높이 확인 필요
		if ((*iter)->GetPosition().y <= 0.5f)
		{
			(*iter)->m_bCollision = false;
			(*iter)->SetPosition(Vec3(0.0f, 20.0f, 0.0f));
			(*iter)->GetPhysicsComponent()->SetWeight(0.0f);
			(*iter)->GetMeshComponent()->SetVisible(false);
			(*iter)->GetShapeComponent()->m_bVisible = false;
			iter = activeSnowList.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
void BettyMovement::DropSnowBall(float _delta)
{
	if (!bDropping)
	{
		snowElapsed += _delta;
		if (snowElapsed > snowOffset && snowDropCount == 0)
		{
			snowDropCount = currentSnowDropCount;
			snowDropElapsed = 0.0f;
			snowElapsed = 0.0f;
			bDropping = true;
		}
	}

	if (snowDropCount > 0)
	{
		snowDropElapsed += _delta;

		if (snowDropElapsed >= snowDropInterval)
		{
			snowDropElapsed = 0.0f;

			// snowball 생성
			float randomX = RandomRange(-20.0f, 20.0f);
			float randomZ = RandomRange(-20.0f, 20.0f);
			Vec3 snowPos = GetOwner()->GetPosition();
			snowPos.x += randomX;
			snowPos.z += randomZ;
			snowPos.y = 20.0f;

			auto snow = GetSnowBall();
			snow->m_bCollision = true;
			snow->GetShapeComponent()->m_bVisible = true;
			snow->GetMeshComponent()->SetVisible(true);
			snow->SetPosition(snowPos);
			activeSnowList.push_back(snow);

			auto pPhysics = snow->GetPhysicsComponent();
			pPhysics->SetWeight(1.0f);

			if (--snowDropCount == 0)
			{
				bDropping = false;
				bSnowControl = false;
			}
		}
	}
}
void BettyMovement::ChangeState(shared_ptr<StateBase> _state)
{
	if (currentState && !currentState->IsInterruptible() && currentState->IsPlaying())
	{
		if (!_state->GetId() == BETTY_S_DEATH)
		{
			return;
		}
	}

	if (currentState)
		currentState->End();

	currentState = _state;

	if (currentState)
	{
		currentState->Enter();
		currentStateId = currentState->GetId();
	}
}
#pragma endregion Snowball

void BettyMovement::HandleAttack(float _delta)
{
	if (distance.Length() < 20.0f)
	{
		// 근접 공격
		auto look = GetOwner()->GetLook();
		float dot = look.Dot(distance);
		// 뒤 ? 무조건 OneHandBack
		if (dot < 0)
		{
			auto next = oneHandBackAttack;
			dynamic_pointer_cast<BettyOneHandBackAttack>(next)->CheckDirection(player.lock()->GetPosition());
			rightRange->m_bCollision = true;
			rightRange->GetShapeComponent()->m_bVisible = true;
			leftRange->m_bCollision = true;
			leftRange->GetShapeComponent()->m_bVisible = true;

			ChangeState(next);
		}
		else
		{
			//meleeIndex = (meleeIndex + 1) % meleeState.size();
			meleeIndex = (int)RandomRange(0.0f, meleeState.size());
			auto next = meleeState[meleeIndex];
			if (next->GetId() == BETTY_S_A_HANDBACK)
			{
				bool right = dynamic_pointer_cast<BettyOneHandBackAttack>(next)->CheckDirection(player.lock()->GetPosition());
				if (right)
				{
					rightRange->m_bCollision = true;
					rightRange->GetShapeComponent()->m_bVisible = true;
				}
				else
				{
					leftRange->m_bCollision = true;
					leftRange->GetShapeComponent()->m_bVisible = true;

				}
			}
			if (next->GetId() == BETTY_S_A_HANDDOWN)
			{
				bool right = dynamic_pointer_cast<BettyOneHandDownAttack>(next)->CheckDirection(player.lock()->GetPosition());
				if (right)
				{
					rightRange->m_bCollision = true;
					rightRange->GetShapeComponent()->m_bVisible = true;
				}
				else
				{
					leftRange->m_bCollision = true;
					leftRange->GetShapeComponent()->m_bVisible = true;

				}
			}
			if (next->GetId() == BETTY_S_A_TWOHAND)
			{
				rightRange->m_bCollision = true;
				rightRange->GetShapeComponent()->m_bVisible = true;
				leftRange->m_bCollision = true;
				leftRange->GetShapeComponent()->m_bVisible = true;
			}
			ChangeState(next);
		}

	}
	else
	{
		// 원거리 공격
		auto nextIndex = (int)RandomRange(0.0f, rangedState.size());
		// 중복 방지
		while (nextIndex == rangedIndex)
		{
			nextIndex = (int)RandomRange(0.0f, rangedState.size());
		}
		rangedIndex = nextIndex;
		auto next = rangedState[rangedIndex];
		if (next->GetId() == BETTY_S_A_ROLL)
		{
			dynamic_pointer_cast<BettyRollAttack>(next)->SetTarget(player);
			GetOwner()->m_szName = L"Enemy";
		}
		if (next->GetId() == BETTY_S_A_JUMPHIGH)
		{
			dynamic_pointer_cast<BettyJumpAttack>(next)->SetTargetPos(player.lock()->GetPosition());
			GetOwner()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
			GetOwner()->m_szName = L"Enemy";
			rightRange->m_bCollision = true;
			rightRange->GetShapeComponent()->m_bVisible = true;
			leftRange->m_bCollision = true;
			leftRange->GetShapeComponent()->m_bVisible = true;
		}
		if (next->GetId() == BETTY_S_A_DROPSNOW)
		{
			// attack 시작 후 1초 후 눈덩이 떨어짐
			bSnowControl = true;
		}
		ChangeState(next);
	}

	auto hp = dynamic_pointer_cast<TCharacter>(GetOwner())->GetHp();
	if (hp < 15 && hp > 10)
	{
		// Phase 2
		currentSnowDropCount = 2;
		bSnowControl = true;
	}

	if (bCanStart3 && hp < 10)
	{
		// Phase 3
		bSnowControl = false;
		bCanStart3 = false;
		currentSnowDropCount = 15;
		snowOffset = 1.5f;
		rangedState.push_back(dropAttack);
	}
}
void BettyMovement::CheckHit()
{
	// HP
	auto comp = dynamic_pointer_cast<TEnemy>(GetOwner());
	bool isHit = comp->CheckHit();
	// hit event 없음. 경직 뭐 아무것도.

	if (comp->IsDead())
	{
		ChangeState(death);
	}

	#pragma region Old
				// 투사체 충돌 확인
	//auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());

	//hitElapsed += TIMER->GetDeltaTime();
	//// 충돌 확인
	//if (hitElapsed > 1.0f) //&& GetOwner()->m_vCollisionList.size() > 0)
	//{
	//	// 근접 공격 확인
	//	bool isCol = false;
	//	if (GetOwner()->m_vCollisionList.size() > 0)
	//	{
	//		auto list = GetOwner()->m_vCollisionList;
	//		for (auto& index : list)
	//		{
	//			if (OBJECT->GetActor(index.first)->m_szName == L"Melee")
	//				isCol = true;
	//		}
	//	}

	//	if (isCol || healthComp->IsHitByProjectile())
	//	{
	//		hitElapsed = 0.0f;
	//		// 피격 시 경직 없고, flashing 뿐
	//		m_fHitFlashTimer = 1.f;  // 1초 동안
	//		m_bIsFlashing = true;

	//		// Anim
	//		healthComp->TakeDamage(1);

	//		if (healthComp->IsDead())
	//		{
	//			currentAction = BettyAction::Die;
	//			ChangeState(death);
	//		}
	//	}
	//}  
#pragma endregion

}
void BettyMovement::AddColliderActor()
{
	// Collider 
	auto body = dynamic_pointer_cast<UMeshComponent>(GetOwner()->GetMeshComponent());
	leftHand = body->GetChildByName(L"LeftHand");
	rightHand = body->GetChildByName(L"RightHand");

	{
		leftRange = make_shared<AActor>();
		leftRange->m_bCollision = false;

		auto collider = make_shared<UBoxComponent>();
		collider->m_bVisible = true;
		collider->SetName(L"Enemy");
		collider->SetLocalScale(Vec3(4.0f, 5.0f, 4.0f));
		collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
		leftRange->SetShapeComponent(collider);

		leftRange->SetPosition(leftHand.lock()->GetWorldPosition());
		leftRange->m_szName = L"Enemy";

		OBJECT->AddActor(leftRange);
		ENEMYCOLLIDER->Add(leftRange);

		collider->m_bVisible = false;
	}

	{
		rightRange = make_shared<AActor>();
		rightRange->m_bCollision = false;

		auto collider = make_shared<UBoxComponent>();
		collider->m_bVisible = true;
		collider->SetName(L"Enemy");
		collider->SetLocalScale(Vec3(4.0f, 5.0f, 4.0f));
		collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
		rightRange->SetShapeComponent(collider);

		rightRange->SetPosition(rightHand.lock()->GetWorldPosition());
		rightRange->m_szName = L"Enemy";

		OBJECT->AddActor(rightRange);
		ENEMYCOLLIDER->Add(rightRange);

		collider->m_bVisible = false;
	}

}

void BettyMovement::Flashing()
{
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
}
void BettyMovement::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
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
