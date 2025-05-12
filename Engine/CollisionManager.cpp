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
	// Ray�� Start Point�� Plane�� �Ÿ�, normal �����ʿ� ������ ���
	float d = _plane.DotNormal(_ray.position);

	Vec3 dir = _ray.direction;
	Vec3 normal = _plane.Normal();
	dir.Normalize();
	normal.Normalize();

	// Ray�� ����� Plane�� Normal ����
	float dot = normal.Dot(dir);

	if (d * dot <= 0)
		return true;

	return false;
}
