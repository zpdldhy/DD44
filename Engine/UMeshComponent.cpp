#include "pch.h"
#include "UMeshComponent.h"

void UMeshComponent::Render()
{
	PreRender();
	PostRender();

	for (auto& child : m_vChild)
	{
		child->Render();
	}
}
