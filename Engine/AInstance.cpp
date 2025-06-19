#include "pch.h"
#include "AInstance.h"
#include "UMeshResources.h"

void AInstance::Init()
{

}

void AInstance::Tick()
{
}

void AInstance::Render()
{
	for (UINT iMeshIndex = 0;iMeshIndex< m_vMeshList.size();iMeshIndex++)
	{
		vector<INSTANCE_VERTEX> vInstanceList;

		// Transform을 모아요
		for (auto iter = m_vMeshList[iMeshIndex].begin(); iter!= m_vMeshList[iMeshIndex].end(); )
		{
			auto pTransform = *iter;
			if (pTransform->GetOwner() == nullptr || 
				pTransform->GetOwner()->m_bDelete == true)
			{
				iter = m_vMeshList[iMeshIndex].erase(iter);
				continue;
			}

			if (pTransform->GetOwner()->m_bRender == false)
			{
				iter++;
				continue;
			}

			INSTANCE_VERTEX world;
			world.matWorld = pTransform->GetWorld();
			world.color = pTransform->GetInstanceColor();
			world.uv = pTransform->GetInstanceUV();
			vInstanceList.emplace_back(world);
			iter++;
		}

		// Mesh를 Binding 해요
		auto pFirstMesh = m_vMeshList[iMeshIndex][0]->GetMesh();
		pFirstMesh->UpdateInstanceList(vInstanceList);
		pFirstMesh->Bind();

		m_vMeshList[iMeshIndex][0]->GetMaterial()->Bind();
		
		auto shaderPath = m_vMeshList[iMeshIndex][0]->GetMaterial()->GetShader()->m_szPath;
		m_pShader = SHADER->Get(shaderPath, L"VS_INSTANCE");

		if (pFirstMesh->GetType() == 0)
		{
			m_pInputLayout = INPUTLAYOUT->Get(L"Instance");
			if (m_pInputLayout == nullptr) 
			{
				INPUTLAYOUT->CreateInstance(m_pShader->m_pCode);
				m_pInputLayout = INPUTLAYOUT->Get(L"Instance");
			}
		}
		//else if (pFirstMesh->GetType() == 1)
		//{
		//	m_pInputLayout = INPUTLAYOUT->Get(L"InstanceIW");
		//	if (m_pInputLayout == nullptr)
		//	{
		//		INPUTLAYOUT->CreateInstanceIW(m_pShader->m_pCode);
		//		m_pInputLayout = INPUTLAYOUT->Get(L"InstanceIW");
		//	}
		//}

		DC->IASetInputLayout(m_pInputLayout->m_pInputLayout.Get());
		DC->VSSetShader(m_pShader->m_pVertexShader.Get(), nullptr, 0);

		// Draw Call 해요
		if (pFirstMesh->GetIndexCount() <= 0)
			DC->DrawInstanced(pFirstMesh->GetVertexCount(), vInstanceList.size(), 0, 0);
		else
			DC->DrawIndexedInstanced(pFirstMesh->GetIndexCount(), vInstanceList.size(), 0, 0, 0);
	}
}

void AInstance::Destroy()
{
	m_vMeshList.clear();
}

void AInstance::SetInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom)
{
	// 제일 상단에 MeshResource가 있다면
	if (_pMeshCom->GetMesh())
	{
		vector<shared_ptr<UMeshComponent>> vTransformList;

		vTransformList.emplace_back(_pMeshCom);

		m_vMeshList.emplace_back(vTransformList);

		_pMeshCom->SetUseInstance(true);
	}

	// 자식 Mesh들 재귀 시작
	InsertChildsMesh(_pMeshCom->GetChildren());
}

void AInstance::AddInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom)
{
	UINT iMeshCount = 0;

	if (m_vMeshList.size() <= 0)
	{
		SetInstanceMesh(_pMeshCom);
		return;
	}

	// 제일 상단에 MeshResource가 있다면
	if (_pMeshCom->GetMesh())
	{
		m_vMeshList[iMeshCount].emplace_back(_pMeshCom);

		_pMeshCom->SetUseInstance(true);
	}

	// 자식 Mesh들 재귀 시작
	InsertChildsMesh(_pMeshCom->GetChildren(), iMeshCount);
}

void AInstance::InsertChildsMesh(vector<shared_ptr<UMeshComponent>> _vChildsList)
{
	for (auto& pChildCom : _vChildsList)
	{
		if (pChildCom->GetMesh())
		{
			vector<shared_ptr<UMeshComponent>> vTransformList;

			vTransformList.emplace_back(pChildCom);

			m_vMeshList.emplace_back(vTransformList);

			pChildCom->SetUseInstance(true);
		}

		InsertChildsMesh(pChildCom->GetChildren());
	}
}

void AInstance::InsertChildsMesh(vector<shared_ptr<UMeshComponent>> _vChildsList, UINT& _iMeshCount)
{
	for (auto& pChildCom : _vChildsList)
	{
		if (pChildCom->GetMesh())
		{
			m_vMeshList[_iMeshCount].emplace_back(pChildCom);

			pChildCom->SetUseInstance(true);
		}

		InsertChildsMesh(pChildCom->GetChildren(), _iMeshCount);
	}
}
