#include "pch.h"
#include "StageManager.h"
#include "TEnemy.h"
#include "TInteractable.h"
#include "EventManager.h"
#include "PrefabToActor.h"

#include "Input.h"

void StageManager::AddEnemyInStage(UINT _stageId, shared_ptr<AActor> _actor)
{
	if (_stageId > stageList.size())
	{
		//assert(false);
	}

	stageList[_stageId].emplace_back(_actor->m_Index);
	auto enemy = dynamic_pointer_cast<TEnemy>(_actor);
	if (enemy)
	{
		enemy->SetStageId(_stageId);
	}
}

void StageManager::AddEnemiesInStage(UINT _stageId, vector<shared_ptr<AActor>> _actorList)
{
	if (_stageId > stageList.size())
	{
		//assert(false);
	}

	for (auto actor : _actorList)
	{
		stageList[_stageId].emplace_back(actor->m_Index);
		auto enemy = dynamic_pointer_cast<TEnemy>(actor);
		if (enemy)
		{
			enemy->SetStageId(_stageId);
		}
	}
}

void StageManager::DeleteEnemyInStage(UINT _stageId, UINT _actorId)
{
	if (_stageId > stageList.size())
	{
		//assert(false);
	}
	// �� �ٷ� ����� ? 
	//m_freeIndices.emplace(make_pair(_stageId, _actorId));
	for (auto iter = stageList[_stageId].begin(); iter != stageList[_stageId].end();)
	{
		if (*iter == _actorId)
		{
			iter = stageList[_stageId].erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void StageManager::EnterFinalStage()
{
	currentPhase = StagePhase::FINAL;
}

void StageManager::Init()
{
	// ���� ���� ����
	stageList.resize(3);

	// final stage ���� Ȯ���� ���� �̺�Ʈ 
	EVENT->AddStageEvent(L"Enter_Final", [this]() {
		this->EnterFinalStage();
		});
}

void StageManager::Tick()
{
	switch (currentPhase)
	{
	case StagePhase::STAGE00:
	{
		if (stageList[0].size() <= 0)
		{
			// �潺 ������
			EVENT->TriggerEvent(EventType::EVENT_FENCE, L"I_Fence2");

			// ���� ������
			currentPhase = StagePhase::STAGE01;
		}
	}
	break;
	case StagePhase::STAGE01:
	{
		if (stageList[1].size() <= 0)
		{
			// ��ٸ� ������
			EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder2");
			EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder1");

			// ���� ������
			currentPhase = StagePhase::STAGE10;
		}
	}
	break;
	case StagePhase::STAGE10:
	{
		if (stageList[2].size() <= 0)
		{
			// �潺 ������
			//if (INPUT->GetButton(H))
			{
				EVENT->TriggerEvent(EventType::EVENT_FENCE, L"I_Fence1");
				currentPhase = StagePhase::STAGE20;

			}

			// ���� ������
		}
	}
	case StagePhase::STAGE20:
	{

	}
	break;
	case StagePhase::FINAL:
	{

	}
	break;
	case StagePhase::DONE:
		break;
	default:
		break;
	}

}

void StageManager::Destroy()
{

}