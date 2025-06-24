#include "pch.h"
#include "TInteractable.h"
#include "TInteractScriptComponent.h"
#include "ObjectManager.h"

void TInteractable::Init()
{
	AActor::Init();
	auto script = GetScriptList()[0];
	interactionScript = dynamic_pointer_cast<TInteractScriptComponent>(script);
}

void TInteractable::Tick()
{
	// 플레이어랑 충돌 확인
	if (m_vCollisionList.size() > 0)
	{
		auto list = m_vCollisionList;
		for (auto& index : list)
		{
			if (OBJECT->GetActor(index.first)->m_szName == L"MyCharacter")
			{
				Interact();
			}
		}
	}

	AActor::Tick();

}

void TInteractable::Interact()
{
	if (!interactionScript)
	{
		return;
	}
	interactionScript->Interact();
}
