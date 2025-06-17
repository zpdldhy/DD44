#include "pch.h"
#include "UMeshComponent.h"
#include "ShadowManager.h"


void UMeshComponent::Render()
{
	if (bRender && bUseInstance == false)
	{
		PreRender();
		PostRender();
		ClearBind();
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
		auto shadowShader = SHADER->Get(L"../Resources/Shader/Shadow.hlsl");
		DC->VSSetShader(shadowShader->GetVS().Get(), nullptr, 0);
		DC->PSSetShader(shadowShader->GetPS().Get(), nullptr, 0);
		PostRender();

	}

	for (auto& child : m_vChild)
	{
		child->RenderShadow();
	}

	//// 1. 깊이 전용 셰이더 설정
	//m_pMaterial->SetShader(L"../Resources/Shader/DepthOnly.hlsl");

	//// 2. Light ViewProj 곱한 행렬 전달
	//Matrix matWVP = m_pOwner->GetWorld() * SHADOWMANAGER->GetLightViewProj();
	//m_pMaterial->SetMatrix("g_matWVP", matWVP);

	//// 3. 실제 렌더링
	//m_pMaterial->Apply(DC);
	//m_pMesh->Draw();
}

void UMeshComponent::ClearBind()
{
	ID3D11Buffer* nullBuffers[1] = { nullptr };

	DC->VSSetConstantBuffers(2, 1, nullBuffers);
	DC->PSSetConstantBuffers(2, 1, nullBuffers);

	DC->VSSetConstantBuffers(4, 1, nullBuffers);
	DC->PSSetConstantBuffers(4, 1, nullBuffers);
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
