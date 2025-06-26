#pragma once
#include "Singleton.h"

class AActor;

class EnemyCollisionManager : public Singleton<EnemyCollisionManager>
{
public:
	EnemyCollisionManager() = default;
	~EnemyCollisionManager() = default;
public:
	vector<shared_ptr<AActor>> enemyList;
	vector<shared_ptr<AActor>> triggerObjectList;
public:
	void Add(shared_ptr<AActor> _col) { enemyList.push_back(_col); }
	void AddObject(shared_ptr<AActor> _col) { triggerObjectList.push_back(_col);}
};

