#pragma once
#include "Singleton.h"

class TEnemy;
class AActor; 
// STAGE00 : ~ stage0 펜스 열리기 전
// STAGE01 : ~ stage0 사다리 열리기 전
// STAGE10 : ~ stage1 잡몹전 끝내기 전
// STAGE20 : ~ 베티
enum class StagePhase { STAGE00, STAGE01, STAGE10, STAGE20, DONE };

class StageManager : public Singleton<StageManager>
{
private:
	vector<vector<UINT>> stageList;
	queue<pair<UINT, UINT>> m_freeIndices;

	vector<shared_ptr<AActor>> triggerList;

	// Phase
	StagePhase currentPhase = StagePhase::STAGE10;

public:
	void AddEnemyInStage(UINT _stageId, shared_ptr<AActor> _actor);
	void AddEnemiesInStage(UINT _stageId, vector<shared_ptr<AActor>> _actorList);
	void DeleteEnemyInStage(UINT _stageId, UINT _actorId);
public:
	void Init();
	void Tick();
	void Destroy();
public:
	StagePhase GetCurrentStage() { return currentPhase; }
};

