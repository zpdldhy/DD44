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
	// Ray의 Start Point와 Plane의 거리
	float d = _plane.DotNormal(_ray.position);
	// Ray의 방향과 Plane의 Normal 내적	

	// 평면 위의 점
	if (d == 0) return true;

	// 법선과 같은 방향
	else if (d > 0)
	{

	}

	// 
	else
	{

	}

	return true;
}
