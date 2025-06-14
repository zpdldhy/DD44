#include "pch.h"
#include "FenceScript.h"

#include "EventManager.h"
#include "Timer.h"

void FenceScript::Init()
{
	originPos = GetOwner()->GetPosition();
	// ��ũ��Ʈ�� ��������� �ٸ��� ������ �� �ִ� ����� ���� ... .. .. . 
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
