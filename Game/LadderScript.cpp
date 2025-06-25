#include "pch.h"
#include "LadderScript.h"
#include "Timer.h"

#include "EventManager.h"

void LadderScript::Init()
{
	// 콜라이더 설정
	GetOwner()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	GetOwner()->GetShapeComponent()->SetName(L"Ladder");

	if (GetOwner()->m_eActorType == ActorType::AT_INTERACTABLE)
	{
		GetOwner()->m_bCollision = false;
		GetOwner()->m_bRender = true;
		auto name = GetOwner()->m_szName;

		EVENT->AddLadderEvent(name, [this]() {
			this->Interact();
			});
		auto triggerMesh = GetOwner()->GetMeshComponent();
		auto children = triggerMesh->GetChildren();
		for (int i = children.size() - 2; i >= 0; i--)
		{
			children[i]->SetVisible(false);

		}
	}

	auto triggerMesh = GetOwner()->GetMeshComponent();
	auto children = triggerMesh->GetChildren();
	activeChildNum = children.size() - 1;
}

void LadderScript::Tick()
{
	if (m_bAppear)
	{
		m_elapsed += TIMER->GetDeltaTime();
		if (m_elapsed > 0.3f)
		{
			auto triggerMesh = GetOwner()->GetMeshComponent();
			auto children = triggerMesh->GetChildren();
			if (activeChildNum < 0)
			{
				activeChildNum = 0;
				m_bAppear = false;
				return;
			}
			children[activeChildNum--]->SetVisible(true);
		}
	}
}

shared_ptr<UScriptComponent> LadderScript::Clone()
{
	auto script = make_shared<LadderScript>();
	return script;
}

void LadderScript::Interact()
{
	// 생기고 사라지고를 여기서 하고
	GetOwner()->m_bCollision = true;
	GetOwner()->m_bRender = true;
	m_bAppear = true;
	// 플레이어 움직이는 거 관련해서는 PlayerMoveScript

}
