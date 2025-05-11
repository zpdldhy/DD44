#pragma once
#include "Singleton.h"

class CollisionManager : public Singleton<CollisionManager>
{
	vector<shared_ptr<class AActor>> m_vCollisionActor;

public:
	void Tick();
	void Destroy();

public:
	void AddCollisionActor(shared_ptr<class AActor> _pActor) { m_vCollisionActor.push_back(_pActor); }
	void AddCollisionActorList(vector<shared_ptr<class AActor>> _vActorList){m_vCollisionActor = _vActorList; }

public:
	static bool RayToPlane(const Ray& _ray, const Plane& _plane);
};

