#include "pch.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "UMeshComponent.h"
#include "UMaterial.h"
#include "DxState.h"

UINT ObjectManager::ActorCount = 0;

void ObjectManager::Tick()
{
	for (auto pActor = m_vActorList.begin();pActor!=m_vActorList.end();)
	{
		// 죽는건가
		if (pActor->second->IsDelete() == true)
			pActor = m_vActorList.erase(pActor);
		else
		{
			pActor->second->Tick();
			m_vRenderActorList.emplace_back(pActor->second);	// 임시 사용.
			pActor++;
		}		
	}
}

void ObjectManager::Render()
{
	CheckStencilList();

	for (auto& pRenderActor : m_vPreRenderActorList)
	{
		auto pMesh = pRenderActor->GetMeshComponent<UMeshComponent>();

		// No Stencil
		if (pMesh == nullptr || pMesh->GetMaterial() == nullptr)
		{
			pRenderActor->Render();
			continue;
		}

		// [1] Actor1 먼저 정상 렌더링 (깊이, 스텐실 기록 X)
		pMesh->GetMaterial()->SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
		pRenderActor->Render();

		// [2] Actor1 위치에 스텐실 = 1 설정 (깊이 테스트는 하되 기록 X)
		if (!pMesh->GetMaterial()->IsUseStencil())
		{
			pMesh->GetMaterial()->SetRenderMode(ERenderMode::Default);
			DC->OMSetDepthStencilState(STATE->m_pDSS_StencilWrite.Get(), 1);
			pRenderActor->Render();
		}
	}

	for (auto& pRenderActor : m_vPostRenderActorList)
	{
		auto pMesh = pRenderActor->GetMeshComponent<UMeshComponent>();

		// No Stencil
		if (pMesh == nullptr || pMesh->GetMaterial() == nullptr)
		{
			pRenderActor->Render();
			continue;
		}

		// [3] Actor2 실루엣으로 스텐실 == 1 영역만 출력 (깊이 Disable)
		pMesh->GetMaterial()->SetRenderMode(ERenderMode::Silhouette);
		DC->OMSetDepthStencilState(STATE->m_pDSS_StencilMaskEqual.Get(), 1);
		pRenderActor->Render();

		// [4] Actor2 일반 렌더링 (깊이 Enable)
		pMesh->GetMaterial()->SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
		pRenderActor->Render();
	}

	ClearRenderList();
}

void ObjectManager::Destroy()
{
	m_vActorList.clear();
	ClearRenderList();
	ActorCount = 0;
}

void ObjectManager::AddActor(shared_ptr<class AActor> _pActor)
{
	_pActor->SetActorIndex(ActorCount);
	_pActor->Init();
	m_vActorList.insert(make_pair(ActorCount, _pActor));

	ActorCount++;
}

void ObjectManager::AddActorList(vector<shared_ptr<class AActor>> _vActorList)
{
	for (auto& pActor : _vActorList)
	{
		pActor->SetActorIndex(ActorCount);
		pActor->Init();
		m_vActorList.insert(make_pair(ActorCount, pActor));

		ActorCount++;
	}
}

shared_ptr<class AActor> ObjectManager::GetActor(UINT _iIndex)
{
	return m_vActorList.find(_iIndex)->second;
}

void ObjectManager::CheckStencilList()
{
	for (auto& pActor : m_vRenderActorList)
	{
		auto pMesh = pActor->GetMeshComponent<UMeshComponent>();
		auto pMat = (pMesh ? pMesh->GetMaterial() : nullptr);

		// 기본 예외 처리
		if (pMesh == nullptr || pMat == nullptr)
		{
			m_vPreRenderActorList.emplace_back(pActor);
			continue;
		}

		// 스텐실용 추가
		if (pMat->IsUseStencil())
			m_vPostRenderActorList.emplace_back(pActor);
		
		if (pMat->IsUseBloom())
			m_vBloomActorList.emplace_back(pActor);

		m_vPreRenderActorList.emplace_back(pActor);
	}
}

void ObjectManager::ClearRenderList()
{
	m_vRenderActorList.clear();
	m_vPreRenderActorList.clear();
	m_vPostRenderActorList.clear();
	m_vBloomActorList.clear();
}
