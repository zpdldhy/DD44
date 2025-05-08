#include "pch.h"
#include "ObjectManager.h"
#include "AActor.h"

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
	for (auto& pRenderActor : m_vRenderActorList)
		pRenderActor->Render();

	m_vRenderActorList.clear();
}

void ObjectManager::Destroy()
{
	m_vActorList.clear();
	m_vRenderActorList.clear();
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