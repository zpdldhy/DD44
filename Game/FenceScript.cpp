#include "pch.h"
#include "FenceScript.h"

#include "EventManager.h"
#include "Timer.h"
#include "UBoxComponent.h"
#include "ObjectManager.h"
#include "EnemyCollisionManager.h"

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
	bodyCollider->SetPosition(GetOwner()->GetPosition() + Vec3(0, 2, 0));
	bodyCollider->SetScale(Vec3(20.0f, 10.0f, 2.0f));
	bodyCollider->SetRotation(GetOwner()->GetRotation());
	auto collider = make_shared<UBoxComponent>();
	collider->m_bVisible = true;
	collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
	bodyCollider->SetShapeComponent(collider);
	OBJECT->AddActor(bodyCollider);
	ENEMYCOLLIDER->Add(bodyCollider);
}

void FenceScript::Tick()
{
	if (m_bOpen)
	{
		m_elapsed += TIMER->GetDeltaTime();
		GetOwner()->AddPosition(Vec3(0, -1, 0) * 0.1f);
		bodyCollider->SetPosition(GetOwner()->GetPosition() + Vec3(0, 2, 0));
		if (m_elapsed > 3.0f)
		{
			m_elapsed = 0.0f;
			m_bOpen = false;
		}
	}
	else if (m_bClose)
	{
		m_elapsed += TIMER->GetDeltaTime();

		if (m_elapsed < 3.0f)
		{
			return;
		}

		GetOwner()->AddPosition(Vec3(0, 1, 0) * 0.1f);
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

	if (GetOwner()->m_szName == L"Fence3")
	{
		EVENT->TriggerEvent(EventType::EVENT_STAGE, L"Enter_Final");
	}
}

void FenceScript::Open()
{
	m_bOpen = true;
}
