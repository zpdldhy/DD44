#include "pch.h"
#include "CollisionManager.h"

void CollisionManager::Tick()
{
	m_vCollisionActor.clear();
}

void CollisionManager::Destroy()
{
	m_vCollisionActor.clear();
}

bool CollisionManager::RayToPlane(const Ray& _ray, const Plane& _plane)
{
	// Ray의 Start Point와 Plane의 거리, normal 방향쪽에 있으면 양수
	float d = _plane.DotNormal(_ray.position);

	Vec3 dir = _ray.direction;
	Vec3 normal = _plane.Normal();
	dir.Normalize();
	normal.Normalize();

	// Ray의 방향과 Plane의 Normal 내적
	float dot = normal.Dot(dir);

	if (d * dot <= 0)
		return true;

	return false;
}
