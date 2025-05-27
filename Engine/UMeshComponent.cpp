#include "pch.h"
#include "UMeshComponent.h"


void UMeshComponent::Render()
{
	if (bRender)
	{
		PreRender();
		PostRender();

	}

	for (auto& child : m_vChild)
	{
		child->Render();
	}
}

void UMeshComponent::AddChild(shared_ptr<UMeshComponent> _child)
{
	m_vChild.push_back(_child);
}

void UMeshComponent::RemoveChild(int _index)
{
	auto child = GetChild(_index);
	m_vChild.erase(m_vChild.begin() + _index);

}

void UMeshComponent::RemoveChild(const wstring& _name)
{
	

}

int UMeshComponent::GetChildIndex(shared_ptr<UMeshComponent> _child)
{
	for (int i = 0; i < m_vChild.size(); i++)
	{
		if (m_vChild[i] == _child)
		{
			return i;
		}
	}
	// 없는 자식을 참조하려함
	return -1;
}

shared_ptr<UMeshComponent> UMeshComponent::GetMeshByName(const wstring& _name)
{
	for (auto& child : m_vChild)
	{
		if (child->GetName() == _name)
		{
			return child;
		}

		auto found = child->GetMeshByName(_name);
		if (found)
		{
			return found;
		}
	}

	return nullptr;
}
