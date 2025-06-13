#include "pch.h"
#include "UMeshComponent.h"
#include "ShadowManager.h"


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

void UMeshComponent::RenderShadow()
{
	if (bRender)
	{
		PreRender();
		// Shader�� �ٲ� �ش�.		
		auto pShader = SHADER->Get(L"../Resources/Shader/DepthOnly.hlsl")->m_pVertexShader.Get();
		DC->VSSetShader(pShader, nullptr, 0);
		PostRender();

	}

	for (auto& child : m_vChild)
	{
		child->Render();
	}

	//// 1. ���� ���� ���̴� ����
	//m_pMaterial->SetShader(L"../Resources/Shader/DepthOnly.hlsl");

	//// 2. Light ViewProj ���� ��� ����
	//Matrix matWVP = m_pOwner->GetWorld() * SHADOWMANAGER->GetLightViewProj();
	//m_pMaterial->SetMatrix("g_matWVP", matWVP);

	//// 3. ���� ������
	//m_pMaterial->Apply(DC);
	//m_pMesh->Draw();
}

void UMeshComponent::RemoveChild(int _index)
{
	auto child = GetChild(_index);
	m_vChild.erase(m_vChild.begin() + _index);

}

bool UMeshComponent::RemoveChild(const wstring& _name)
{
		for (auto it = m_vChild.begin(); it != m_vChild.end(); ++it) {
			if ((*it)->GetName() == _name) {
				m_vChild.erase(it);
				return true;
			}
		}

		for (auto& child : m_vChild) {
			if (child->RemoveChild(_name)) {
				return true;
			}
		}

		return false;
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
	// ���� �ڽ��� �����Ϸ���
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
