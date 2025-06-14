#include "pch.h"
#include "LadderScript.h"
#include "Timer.h"

#include "EventManager.h"

void LadderScript::Init()
{
	// 콜라이더 설정
	GetOwner()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);

	if (GetOwner()->m_szName == L"I_Ladder")
	{
		GetOwner()->m_bCollision = false;
		GetOwner()->m_bRender = false;
		EVENT->AddLadderEvent(0, [this]() {
			this->Interact();
			});
		auto triggerMesh = GetOwner()->GetMeshComponent();
		auto children = triggerMesh->GetChildren();
		for (int i = 0; i < children.size(); i++)
		{
			children[i]->SetVisible(false);

		}
	}
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
			if (activeChildNum >= children.size())
			{
				activeChildNum = 0;
				m_bAppear = false;
				return;
			}
			children[activeChildNum++]->SetVisible(true);
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
