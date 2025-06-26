#pragma once
#include "Singleton.h"

class TEnemy;
class AActor; 
// STAGE00 : ~ stage0 �潺 ������ ��
// STAGE01 : ~ stage0 ��ٸ� ������ ��
// STAGE10 : ~ stage1 ����� ������ ��
// STAGE20 : ~ ��Ƽ ���� ��
// STAGE21 : ~ ��Ƽ ��� ����
enum class StagePhase { STAGE00, STAGE01, STAGE10, STAGE20, FINAL, DONE };

class StageManager : public Singleton<StageManager>
{
private:
	vector<vector<UINT>> stageList;
	queue<pair<UINT, UINT>> m_freeIndices;

	// Phase
	StagePhase currentPhase = StagePhase::STAGE00;

public:
	void AddEnemyInStage(UINT _stageId, shared_ptr<AActor> _actor);
	void AddEnemiesInStage(UINT _stageId, vector<shared_ptr<AActor>> _actorList);
	void DeleteEnemyInStage(UINT _stageId, UINT _actorId);
	void EnterFinalStage();
public:
	void Init();
	void Tick();
	void Destroy();
public:
	StagePhase GetCurrentStage() { return currentPhase; }
};

