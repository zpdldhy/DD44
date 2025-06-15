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
	for (auto& pMesh : m_vActorTransformList)
	{
		vector<INSTANCE_VERTEX> vInstanceList;

		// Transform을 모아요
		for (auto& pTransform : pMesh)
		{
			INSTANCE_VERTEX world;
			world.matWorld = pTransform->GetWorld();
			vInstanceList.emplace_back(world);
		}

		// Mesh를 Binding 해요
		auto pFirstMesh = pMesh[0]->GetMesh();
		pFirstMesh->UpdateInstanceList(vInstanceList);
		pFirstMesh->Bind();

		pMesh[0]->GetMaterial()->Bind();
		
		auto shaderPath = pMesh[0]->GetMaterial()->GetShader()->m_szPath;
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
		else if (pFirstMesh->GetType() == 1)
		{
			m_pInputLayout = INPUTLAYOUT->Get(L"InstanceIW");
			if (m_pInputLayout == nullptr)
			{
				INPUTLAYOUT->CreateInstanceIW(m_pShader->m_pCode);
				m_pInputLayout = INPUTLAYOUT->Get(L"InstanceIW");
			}
		}

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
	m_vActorTransformList.clear();
}

void AInstance::SetInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom)
{
	// 제일 상단에 MeshResource가 있다면
	if (_pMeshCom->GetMesh())
	{
		vector<shared_ptr<UMeshComponent>> vTransformList;
		vTransformList.emplace_back(_pMeshCom);
		m_vActorTransformList.emplace_back(vTransformList);
		_pMeshCom->SetUseInstance(true);
	}

	// 자식 Mesh들 재귀 시작
	InsertChildsMesh(_pMeshCom->GetChildren());
}

void AInstance::AddInstanceTransform(shared_ptr<UMeshComponent> _pMeshCom)
{
	UINT iMeshCount = 0;

	// 제일 상단에 MeshResource가 있다면
	if (_pMeshCom->GetMesh())
	{
		m_vActorTransformList[iMeshCount++].emplace_back(_pMeshCom);
		_pMeshCom->SetUseInstance(true);
	}

	// 자식 Mesh들 재귀 시작
	InsertChildsTransform(_pMeshCom->GetChildren(), iMeshCount);
}

void AInstance::InsertChildsMesh(vector<shared_ptr<UMeshComponent>> _vChildsList)
{
	for (auto& pChildCom : _vChildsList)
	{
		if (pChildCom->GetMesh())
		{
			vector<shared_ptr<UMeshComponent>> vTransformList;
			vTransformList.emplace_back(pChildCom);
			m_vActorTransformList.emplace_back(vTransformList);
			pChildCom->SetUseInstance(true);
		}

		InsertChildsMesh(pChildCom->GetChildren());
	}
}

void AInstance::InsertChildsTransform(vector<shared_ptr<UMeshComponent>> _vChildsList, UINT& _iMeshCount)
{
	for (auto& pChildCom : _vChildsList)
	{
		if (pChildCom->GetMesh())
		{
			m_vActorTransformList[_iMeshCount++].emplace_back(pChildCom);
			pChildCom->SetUseInstance(true);
		}

		InsertChildsTransform(pChildCom->GetChildren(), _iMeshCount);
	}
}
