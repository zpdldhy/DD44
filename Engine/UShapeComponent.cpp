#include "pch.h"
#include "UShapeComponent.h"

void UShapeComponent::CollisionAction()
{
	if (m_CollisionScript == nullptr) return;

	if (m_CollisionEnabled == CollisionEnabled::CE_NOCOLLISION) 
		return;

	else if (m_CollisionEnabled == CollisionEnabled::CE_QUERYONLY)
	{
		m_CollisionScript->QueryOnly();
	}
	else if (m_CollisionEnabled == CollisionEnabled::CE_QUERYANDPHYSICS)
	{
		m_CollisionScript->QueryAndPhysics();
	}
}
