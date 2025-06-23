#pragma once
#include "Singleton.h"

class TEnemy;
class AActor; 

class StageManager : public Singleton<StageManager>
{
private:
	vector<vector<UINT>> stageList;
	queue<pair<UINT, UINT>> m_freeIndices;

	vector<shared_ptr<AActor>> triggerList;

	// Phase
	enum StagePhase { STAGE00, STAGE01, STAGE10, DONE };
	StagePhase currentPhase = STAGE10;
public:
	void AddEnemyInStage(UINT _stageId, shared_ptr<AActor> _actor);
	void AddEnemiesInStage(UINT _stageId, vector<shared_ptr<AActor>> _actorList);
	void DeleteEnemyInStage(UINT _stageId, UINT _actorId);
public:
	void Init();
	void Tick();
	void Destroy();
};

