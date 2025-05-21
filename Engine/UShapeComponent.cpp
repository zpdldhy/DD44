#include "pch.h"
#include "UShapeComponent.h"

void UShapeComponent::Init()
{
	USceneComponent::Init();

	for (auto& child : m_vChildren)
	{
		child->SetOwner(GetOwner());
		child->Init();
	}
}

void UShapeComponent::Tick()
{
	USceneComponent::Tick();

	for (auto& child : m_vChildren)
	{
		if (child)
		{
			child->Tick();
		}
	}
}

void UShapeComponent::Render()
{
	USceneComponent::Render();

	for (auto& child : m_vChildren)
	{
		if (child)
		{
			child->Render();
		}
	}
}

void UShapeComponent::PreRender()
{
}

void UShapeComponent::PostRender()
{
}

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

void UShapeComponent::AddChild(std::shared_ptr<UShapeComponent> child)
{
	if (child)
	{
		m_vChildren.push_back(child);
	}
}

bool UShapeComponent::HasChildren() const
{
	return !m_vChildren.empty();
}
