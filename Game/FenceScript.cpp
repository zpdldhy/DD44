#include "pch.h"
#include "FenceScript.h"

#include "EventManager.h"
#include "Timer.h"
#include "UBoxComponent.h"
#include "ObjectManager.h"
#include "EnemyCollisionManager.h"

#include "Sound.h"
#include "EffectManager.h"

void FenceScript::Init()
{
	originPos = GetOwner()->GetPosition();

	if (GetOwner()->m_eActorType == ActorType::AT_NONE)
	{
		EVENT->AddFenceEvent(GetOwner()->m_szName, [this]() {
			this->Open();
			});
		GetOwner()->m_bCollision = false;
	}

	// 얘도 콜라이더 2개 필요함
	// trigger 콜라이더
	GetOwner()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);

	// 몸체 콜라이더
	bodyCollider = make_shared<AActor>();
	bodyCollider->m_szName = L"FenceCollider";
	bodyCollider->SetPosition(GetOwner()->GetPosition() + Vec3(0, 2, 0));
	bodyCollider->SetScale(Vec3(20.0f, 10.0f, 2.0f));
	bodyCollider->SetRotation(GetOwner()->GetRotation());
	auto collider = make_shared<UBoxComponent>();
	collider->m_bVisible = true;
	collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
	bodyCollider->SetShapeComponent(collider);
	OBJECT->AddActor(bodyCollider);
	ENEMYCOLLIDER->AddObject(bodyCollider);
}

void FenceScript::Tick()
{
	auto dt = TIMER->GetDeltaTime();
	if (m_bOpen)
	{
		m_elapsed += dt;
		m_dustElapsed += dt;
		GetOwner()->AddPosition(Vec3(0, -1, 0) * 5.0f * dt);
		bodyCollider->SetPosition(GetOwner()->GetPosition() + Vec3(0, 2, 0));
		if (m_elapsed > 3.0f)
		{
			m_elapsed = 0.0f;
			m_bOpen = false;
		}

		auto right = GetOwner()->GetRight();
		if (m_dustElapsed >= 0.4f && m_elapsed <= 1.0f)
		{
			//Profiler p("Enter");
			//EFFECT->PlayEffect(EEffectType::PoppingDust, pos, 0, Vec3(0.0f, 0.0f, 0.0f), 0.6f);
			EFFECT->PlayEffect(EEffectType::PoppingDust, dustPos - 8.0f * right, 0, Vec3(0.0f, 0.0f, 0.0f), 0.5f);
			EFFECT->PlayEffect(EEffectType::PoppingDust, dustPos + 8.0f * right, 0, Vec3(0.0f, 0.0f, 0.0f), 0.5f);
			m_dustElapsed = 0.0f;
		}

	}
	else if (m_bClose)
	{
		m_elapsed += dt;

		if (m_elapsed < 3.0f)
		{
			return;
		}

		GetOwner()->AddPosition(Vec3(0, 1, 0) * 2.8f * dt);
		bodyCollider->SetPosition(GetOwner()->GetPosition() + Vec3(0, 2, 0));

		if (m_elapsed > 5.0f)
		{
			m_elapsed = 0.0f;
			m_bClose = false;
		}
	}
}

shared_ptr<UScriptComponent> FenceScript::Clone()
{
	auto script = make_shared<FenceScript>();
	return script;
}

void FenceScript::Interact()
{
	// 닫기
	m_bClose = true;
	SOUND->GetPtr(ESoundType::Close_Fence)->PlayEffect2D();

	if (GetOwner()->m_szName == L"Fence3")
	{
		EVENT->TriggerEvent(EventType::EVENT_STAGE, L"Enter_Final");
	}
}

void FenceScript::Open()
{
	m_bOpen = true;
	SOUND->GetPtr(ESoundType::Open_Fence)->PlayEffect2D();

	dustPos = bodyCollider->GetPosition();
}
