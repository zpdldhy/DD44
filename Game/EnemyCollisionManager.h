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
public:
	void Add(shared_ptr<AActor> _col) { enemyList.push_back(_col);}
};

