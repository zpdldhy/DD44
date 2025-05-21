#include "pch.h"
#include "AUIActor.h"
#include "UMaterial.h"

void AUIActor::Tick()
{
	m_pState->ProcessState(this);

	if (m_eStateType == UIStateType::ST_IDLE)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pIdleTexture);
	}
	else if (m_eStateType == UIStateType::ST_HOVER)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pHoverTexture);
	}
	else if (m_eStateType == UIStateType::ST_ACTIVE)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pActiveTexture);
	}
	else if (m_eStateType == UIStateType::ST_SELECT)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pSelectTexture);
	}

	AActor::Tick();
}
