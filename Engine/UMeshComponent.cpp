#include "pch.h"
#include "UMeshComponent.h"
#include "MeshTransform.h"


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
	if (m_pMeshTrans)
	{
		m_pMeshTrans->AddChild(_child->m_pMeshTrans);
		_child->GetMeshTransform()->SetParent(m_pMeshTrans);
	}

}

void UMeshComponent::RemoveChild(int _index)
{
	auto child = GetChild(_index);
	// Transform 정리
	m_vChild.erase(m_vChild.begin() + _index);

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

shared_ptr<UMeshComponent> UMeshComponent::GetChildByName(const wstring& _name)
{
	for (auto& child : m_vChild)
	{
		if (child->GetName() == _name)
		{
			return child;
		}

		auto found = child->GetChildByName(_name);
		if (found)
		{
			return found;
		}
	}

	return nullptr;
}

void UMeshComponent::SetLocalPosition(const Vec3& _pos)
{
	if (m_pMeshTrans)
	{
		m_pMeshTrans->SetLocalPosition(_pos);
	}
	else
	{
		USceneComponent::SetLocalPosition(_pos);
	}
}

void UMeshComponent::SetLocalRotation(const Vec3& _rot)
{
	if (m_pMeshTrans)
	{
		m_pMeshTrans->SetLocalRotation(_rot);
	}
	else
	{
		USceneComponent::SetLocalRotation(_rot);
	}
}

void UMeshComponent::SetLocalScale(const Vec3& _scale)
{
	if (m_pMeshTrans)
	{
		m_pMeshTrans->SetLocalScale(_scale);
	}
	else
	{
		USceneComponent::SetLocalScale(_scale);
	}
}

//shared_ptr<UMeshComponent> UMeshComponent::Clone()
//{
//	shared_ptr<UMeshComponent> meshComp = make_shared<UMeshComponent>();
//
//
//	/*
//	std::wstring m_MeshPath;
//	vector<shared_ptr<UMeshComponent>>	m_vChild;
//	bool bRender = true;
//	shared_ptr<MeshTransform> m_pMeshTrans;
//	m_pMaterial
//	bool m_bUseBoneMatrix = false;
//	cbData
//	m_cbData;
//	*/
//	//meshComp->m_MeshPath = m_MeshPath;
//	//meshComp->m_vChild = m_vChild;
//	//meshComp->bRender = bRender;
//	///
//	//meshComp->m_pMeshTrans = m_pMeshTrans;
//
//		return shared_ptr<UMeshComponent>();
//}
