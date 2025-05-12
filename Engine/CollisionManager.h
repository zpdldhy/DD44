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
	static bool GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& inter);
	static bool PointInPolygon(const Vec3& _inter, const Vec3& _faceNormal, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2);
};

