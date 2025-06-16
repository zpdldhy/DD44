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
	for (UINT iMeshIndex = 0;iMeshIndex< m_vTransformList.size();iMeshIndex++)
	{
		vector<INSTANCE_VERTEX> vInstanceList;

		// Transform을 모아요
		for (auto& pTransform : m_vTransformList[iMeshIndex])
		{
			INSTANCE_VERTEX world;
			world.matWorld = pTransform->GetWorld();
			world.color = pTransform->GetInstanceColor();
			world.uv = pTransform->GetInstanceUV();
			vInstanceList.emplace_back(world);
		}

		// Mesh를 Binding 해요
		auto pFirstMesh = m_vTransformList[iMeshIndex][0]->GetMesh();
		pFirstMesh->UpdateInstanceList(vInstanceList);
		pFirstMesh->Bind();

		m_vTransformList[iMeshIndex][0]->GetMaterial()->Bind();
		
		auto shaderPath = m_vTransformList[iMeshIndex][0]->GetMaterial()->GetShader()->m_szPath;
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
	m_vTransformList.clear();
	m_vColorList.clear();
	m_vUVList.clear();
}

void AInstance::SetInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom)
{
	// 제일 상단에 MeshResource가 있다면
	if (_pMeshCom->GetMesh())
	{
		vector<shared_ptr<UMeshComponent>> vTransformList;
		vector<Color> vColorList;
		vector<Vec4> vUVList;

		vTransformList.emplace_back(_pMeshCom);
		vColorList.emplace_back(_pMeshCom->GetInstanceColor());
		vUVList.emplace_back(_pMeshCom->GetInstanceUV());

		m_vTransformList.emplace_back(vTransformList);
		m_vColorList.emplace_back(vColorList);
		m_vUVList.emplace_back(vUVList);

		_pMeshCom->SetUseInstance(true);
	}

	// 자식 Mesh들 재귀 시작
	InsertChildsMesh(_pMeshCom->GetChildren());
}

void AInstance::AddInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom)
{
	UINT iMeshCount = 0;

	// 제일 상단에 MeshResource가 있다면
	if (_pMeshCom->GetMesh())
	{
		m_vTransformList[iMeshCount].emplace_back(_pMeshCom);
		m_vColorList[iMeshCount].emplace_back(_pMeshCom->GetInstanceColor());
		m_vUVList[iMeshCount++].emplace_back(_pMeshCom->GetInstanceUV());

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
			vector<Color> vColorList;
			vector<Vec4> vUVList;

			vTransformList.emplace_back(pChildCom);
			vColorList.emplace_back(pChildCom->GetInstanceColor());
			vUVList.emplace_back(pChildCom->GetInstanceUV());

			m_vTransformList.emplace_back(vTransformList);
			m_vColorList.emplace_back(vColorList);
			m_vUVList.emplace_back(vUVList);

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
			m_vTransformList[_iMeshCount].emplace_back(pChildCom);
			m_vColorList[_iMeshCount].emplace_back(pChildCom->GetInstanceColor());
			m_vUVList[_iMeshCount++].emplace_back(pChildCom->GetInstanceUV());

			pChildCom->SetUseInstance(true);
		}

		InsertChildsMesh(pChildCom->GetChildren(), _iMeshCount);
	}
}
