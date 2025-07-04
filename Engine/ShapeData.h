#pragma once

enum class ShapeType
{
	ST_NONE,
	ST_BOX,
	ST_SPHERE,
	ST_CAPSULE,
	ST_COUNT,
};

enum class RangeType
{
	RT_BOX,
	RT_SPHERE,
	RT_LOOKRAY,
	RT_DOWNRAY,
	RT_COUNT,
};

struct MouseRay : public Ray
{
	POINT WinPos = { 0, 0 };
	Vec3 EndPos = { 0.f, 0.f, 1.f };

	void Click();
};

struct Shape
{
	Vec3 vCenter = { 0.f, 0.f, 0.f };
};

struct Box : public Shape
{
	// AABB
	Vec3 vMax = { 0.f, 0.f, 0.f };
	Vec3 vMin = { 0.f, 0.f, 0.f };
	// OBB
	Vec3 vAxis[3] = { {0.f, 0.f, 0.f}, };
	float vExtent[3] = {0.f, 0.f, 0.f};

	bool Contains(const Vec3& pos) const
	{
		return (pos.x >= vMin.x && pos.x <= vMax.x &&
			pos.z >= vMin.z && pos.z <= vMax.z);
	}
};

struct Sphere : public Shape
{
	float fRadius = 0.f;
};