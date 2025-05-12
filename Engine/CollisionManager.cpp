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

bool CollisionManager::GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& inter)
{
	float dot1 = _normal.Dot(_ray.direction);
	float dot2 = _normal.Dot(_point - _ray.position);

	float Dt = dot2 / dot1;

	if (Dt < 0 || Dt>1)
		return false;

	inter = _ray.position + _ray.direction * Dt;
	return true;
}

bool CollisionManager::PointInPolygon(const Vec3& _inter, const Vec3& _faceNormal, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2)
{
	Vec3 d0 = _v1 - _v0;
	Vec3 d1 = _v2 - _v1;
	Vec3 d2 = _v0 - _v2;

	Vec3 c0 = d0.Cross(_inter - _v0);
	float d = _faceNormal.Dot(c0);
	if (d < 0.f) return false;

	Vec3 c1 = d1.Cross(_inter - _v1);
	d = _faceNormal.Dot(c1);
	if (d < 0.f) return false;

	Vec3 c2 = d2.Cross(_inter - _v2);
	d = _faceNormal.Dot(c2);
	if (d < 0.f) return false;

	return true;
}
