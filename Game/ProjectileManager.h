#pragma once
#include "Singleton.h"

class AActor;

enum class ProjectileType
{
	PlayerArrow = 0,
	MagicBall,
};

struct ProjectileData
{
	ProjectileType type;
	float maxTime = 5.0f;
	float elapsed;
	Vec3 startPos;
	Vec3 dir;
	bool parry = false;
	float velocity = 0.5f;
	float heightRatio = 0.2f;
	shared_ptr<AActor> projectile;
	void InitData(ProjectileType _type, Vec3 _sPos, Vec3 _dir)
	{
		type = _type;
		elapsed = 0.0f;
		startPos = _sPos;
		dir = _dir;
	}
};

class ProjectileManager : public Singleton<ProjectileManager> 
{
public:
	ProjectileManager() = default;
	~ProjectileManager() = default;
public:	
	vector<ProjectileData> arrowList;
	vector<ProjectileData> magicList;
	vector<ProjectileData> activeObjList;
	
public:
	void Init();
	void Tick();
	void Destroy();

public:
	void Create();
	void ActivateOne(ProjectileType _type, Vec3 _pos, Vec3 _dir);
	void DeactivateOne(ProjectileData _target);
public:
	vector<shared_ptr<AActor>> GetActorList();

//Effect Time
private:
	float m_fMagicSpawnTimer = 0.0f;
	float m_fMagicSpawnDelay = 0.05f;
};

