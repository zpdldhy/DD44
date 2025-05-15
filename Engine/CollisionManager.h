#pragma once
#include "Singleton.h"

enum class CollisionFlag
{

};

class Collision : Singleton<Collision>
{
public:
	static void CheckCollision(vector<UINT> _vActorIndex);

public:
	// Ray
	static bool CheckRayToPlane(const Ray& _ray, const Plane& _plane);
	static bool CheckMousePicking(const Ray& _ray, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2, const Vec3& _normal, Vec3& _inter);
	static bool CheckAABBToRay(const Ray& _ray, const Box& _box, Vec3& _inter);

	static bool GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& _inter);
	static bool PointInPolygon(const Vec3& _inter, const Vec3& _faceNormal, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2);
};

