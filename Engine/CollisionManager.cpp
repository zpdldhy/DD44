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
	// Ray�� Start Point�� Plane�� �Ÿ�
	float d = _plane.DotNormal(_ray.position);
	// Ray�� ����� Plane�� Normal ����	

	// ��� ���� ��
	if (d == 0) return true;

	// ������ ���� ����
	else if (d > 0)
	{

	}

	// 
	else
	{

	}

	return true;
}
