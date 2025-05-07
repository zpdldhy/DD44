#include "pch.h"
#include "ObjectManager.h"
#include "AActor.h"

UINT ObjectManager::ActorCount = 0;

void ObjectManager::Init()
{
	for (auto& pActor : m_vActorList)
	{
		pActor.second->Init();
	}
}

void ObjectManager::Tick()
{
	for (auto& pActor : m_vActorList)
	{
		pActor.second->Tick();
		m_vRenderActorList.emplace_back(pActor.second);	// 임시 사용.
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
	_pActor->SetDestroyFunction(
		[this](std::shared_ptr<AActor> objToRemove) { this->DeleteActor(objToRemove); }
	);

	m_vActorList.insert(make_pair(ActorCount, _pActor));

	ActorCount++;
}

void ObjectManager::AddActorList(vector<shared_ptr<class AActor>> _vActorList)
{
	for (auto& pActor : _vActorList)
	{
		pActor->SetActorIndex(ActorCount);
		pActor->SetDestroyFunction(
			[this](std::shared_ptr<AActor> objToRemove) { this->DeleteActor(objToRemove); }
		);

		m_vActorList.insert(make_pair(ActorCount, pActor));

		ActorCount++;
	}
}

void ObjectManager::DeleteActor(shared_ptr<class AActor> _pActor)
{
	for (auto iter = m_vActorList.begin(); iter != m_vActorList.end();)
	{
		if (iter->first == _pActor->GetActorIndex())
			iter = m_vActorList.erase(iter);
		else
			iter++;
	}
}
