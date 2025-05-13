#pragma once
#include "Singleton.h"

class Collision : Singleton<Collision>
{
public:
	static void CheckCollision(vector<shared_ptr<class AActor>> _vActorList);

public:
	static bool RayToPlane(const Ray& _ray, const Plane& _plane);
	static bool GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& _inter);
	static bool PointInPolygon(const Vec3& _inter, const Vec3& _faceNormal, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2);
	static bool CheckMousePicking(const Ray& _ray, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2, const Vec3& _normal, Vec3& _inter);
};

