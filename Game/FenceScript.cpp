#include "pch.h"
#include "FenceScript.h"

#include "EventManager.h"
#include "Timer.h"

void FenceScript::Init()
{
	originPos = GetOwner()->GetPosition();
	// 스크립트의 멤버변수를 다르게 설정할 수 있는 방법이 없니 ... .. .. . 
	EVENT->AddFenceEvent(GetOwner()->m_szName, [this]() {
		this->Interact();
		});
}

void FenceScript::Tick()
{
	if (m_bOpen)
	{
		m_elapsed += TIMER->GetDeltaTime();
		GetOwner()->AddPosition(Vec3(0, -1, 0) * 0.1f);
		
		if (m_elapsed > 3.0f)
		{
			m_bOpen = false;
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
	m_bOpen = true;
}
