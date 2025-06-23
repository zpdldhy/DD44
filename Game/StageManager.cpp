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
		assert(false);
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
		assert(false);
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
		assert(false);
	}
	// 걍 바로 지울까 ? 
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

void StageManager::Init()
{
	// 개수 설정 주의
	stageList.resize(3);
}

void StageManager::Tick()
{
	switch (currentPhase)
	{
	case StageManager::STAGE00:
	{
		if (stageList[0].size() <= 0)
		{
			// 펜스 내리기
			EVENT->TriggerEvent(EventType::EVENT_FENCE, L"I_Fence2");

			// 다음 페이즈
			currentPhase = STAGE01;
		}
	}
	break;
	case StageManager::STAGE01:
	{
		if (stageList[1].size() <= 0)
		{
			// 사다리 내리기
			EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder2");

			// 다음 페이즈
			currentPhase = STAGE10;
		}
	}
	break;
	case StageManager::STAGE10:
	{
		if (stageList[2].size() <= 0)
		{
			// 펜스 내리기
			if (INPUT->GetButton(H))
			{
				EVENT->TriggerEvent(EventType::EVENT_FENCE, L"I_Fence1");
				currentPhase = DONE;

			}

			// 다음 페이즈
		}
	}
	break;
	case StageManager::DONE:
		break;
	default:
		break;
	}

}

void StageManager::Destroy()
{

}