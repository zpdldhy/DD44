#include "pch.h"
#include "HealerScript.h"
#include "Timer.h"
#include "TPlayer.h"

void HealerScript::Init()
{
	// 콜라이더 설정
	GetOwner()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	GetOwner()->GetShapeComponent()->SetName(L"Healer");

	GetOwner()->m_bCollision = true;
}

void HealerScript::Tick()
{
	float time = TIMER->GetGameTime();

	float currY = cosf(time * 2.0f) * 0.5f;
	float prevY = cosf((time - TIMER->GetDeltaTime()) * 2.0f) * 0.5;
	float deltaY = currY - prevY;

	GetOwner()->AddPosition(Vec3(0.f, deltaY, 0.f));
}

shared_ptr<UScriptComponent> HealerScript::Clone()
{
	auto script = make_shared<HealerScript>();
	return script;
}

void HealerScript::Interact()
{
	GetOwner()->AddRotation(Vec3(0.f, 0.1f, 0.f));
}
