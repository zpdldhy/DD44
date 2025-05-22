#include "pch.h"
#include "UMeshComponent.h"

void UMeshComponent::Render()
{
	if(!bRender) { return; }

	PreRender();
	PostRender();

	for (auto& child : m_vChild)
	{
		child->Render();
	}
}
