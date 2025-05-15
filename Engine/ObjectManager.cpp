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
		// �״°ǰ�
		if (pActor->second->m_bDelete == true)
			pActor = m_vActorList.erase(pActor);
		else
		{
			pActor->second->Tick();
			m_vRenderActorList.emplace_back(pActor->second);	// �ӽ� ���.
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

		// [1] Actor1 ���� ���� ������ (����, ���ٽ� ��� X)
		pMesh->GetMaterial()->SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
		pRenderActor->Render();

		// [2] Actor1 ��ġ�� ���ٽ� = 1 ���� (���� �׽�Ʈ�� �ϵ� ��� X)
		pMesh->GetMaterial()->SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSS_StencilWrite.Get(), 1);
		pRenderActor->Render();
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

		// [3] Actor2 �Ƿ翧���� ���ٽ� == 1 ������ ��� (���� Disable)
		pMesh->GetMaterial()->SetRenderMode(ERenderMode::Silhouette);
		DC->OMSetDepthStencilState(STATE->m_pDSS_StencilMaskEqual.Get(), 1);
		pRenderActor->Render();

		// [4] Actor2 �Ϲ� ������ (���� Enable)
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
	_pActor->m_Index = ActorCount;
	_pActor->Init();
	m_vActorList.insert(make_pair(ActorCount, _pActor));
	m_vActorIndexList.emplace_back(ActorCount);		// �ӽ� ���

	ActorCount++;
}

void ObjectManager::AddActorList(vector<shared_ptr<class AActor>> _vActorList)
{
	for (auto& pActor : _vActorList)
	{
		pActor->m_Index = ActorCount;
		pActor->Init();
		m_vActorList.insert(make_pair(ActorCount, pActor));

		m_vActorIndexList.emplace_back(ActorCount);	// �ӽ� ���
		ActorCount++;
	}
}

void ObjectManager::RemoveActor(std::shared_ptr<class AActor> _pActor)
{
	_pActor->m_bDelete = true;
}

shared_ptr<class AActor> ObjectManager::GetActor(UINT _iIndex)
{
	return m_vActorList.find(_iIndex)->second;
}

const map<UINT, shared_ptr<AActor>>& ObjectManager::GetActorList() const
{
	return m_vActorList;
}

void ObjectManager::CheckStencilList()
{
	for (auto& pActor : m_vRenderActorList)
	{
		auto pMesh = pActor->GetMeshComponent<UMeshComponent>();

		if (pMesh == nullptr || pMesh->GetMaterial() == nullptr)
		{
			m_vPreRenderActorList.emplace_back(pActor);
			continue;
		}

		if (pMesh->GetMaterial()->IsUseStencil() == false)
			m_vPreRenderActorList.emplace_back(pActor);
		else
			m_vPostRenderActorList.emplace_back(pActor);
	}
}

void ObjectManager::ClearRenderList()
{
	m_vRenderActorList.clear();
	m_vPreRenderActorList.clear();
	m_vPostRenderActorList.clear();
}
