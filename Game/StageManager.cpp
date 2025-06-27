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

void StageManager::EnterFinalStage()
{
	currentPhase = StagePhase::FINAL;
}

void StageManager::Init()
{
	// 개수 설정 주의
	stageList.resize(3);

	// final stage 입장 확인을 위한 이벤트 
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
			// 펜스 내리기
			EVENT->TriggerEvent(EventType::EVENT_FENCE, L"I_Fence2");

			// 다음 페이즈
			currentPhase = StagePhase::STAGE01;
		}
	}
	break;
	case StagePhase::STAGE01:
	{
		if (stageList[1].size() <= 0)
		{
			// 사다리 내리기
			EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder2");
			EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder1");

			// 다음 페이즈
			currentPhase = StagePhase::STAGE10;
		}
	}
	break;
	case StagePhase::STAGE10:
	{
		if (stageList[2].size() <= 0)
		{
			// 펜스 내리기
			//if (INPUT->GetButton(H))
			{
				EVENT->TriggerEvent(EventType::EVENT_FENCE, L"I_Fence1");
				currentPhase = StagePhase::STAGE20;

			}

			// 다음 페이즈
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