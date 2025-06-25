#pragma once
#include "Singleton.h"

class TEnemy;
class AActor; 
// STAGE00 : ~ stage0 �潺 ������ ��
// STAGE01 : ~ stage0 ��ٸ� ������ ��
// STAGE10 : ~ stage1 ����� ������ ��
// STAGE20 : ~ ��Ƽ
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

