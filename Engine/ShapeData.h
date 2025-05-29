#pragma once

enum class ShapeType
{
	ST_NONE,
	ST_BOX,
	ST_SPHERE,
	ST_CAPSULE,
};

struct MouseRay : public Ray
{
	POINT WinPos = { 0, 0 };
	Vec3 EndPos = { 0.f, 0.f, 1.f };

	void Click();
};

struct Box
{
	Vec3 vCenter = { 0.f, 0.f, 0.f };
	//Vec3 vVertex[8] = { {0.f, 0.f, 0.f}, };
	// AABB
	Vec3 vMax = { 0.f, 0.f, 0.f };
	Vec3 vMin = { 0.f, 0.f, 0.f };
	// OBB
	Vec3 vAxis[3] = { {0.f, 0.f, 0.f}, };
	float vExtent[3] = {0.f, 0.f, 0.f};

	Box(){}

	Box(Vec3 _min, Vec3 _max)
	{
		Set(_min, _max);
	}

	void Set(Vec3 _min, Vec3 _max)
	{
		vMin = _min;
		vMax = _max;
		vCenter = (_max + _min) * 0.5f;

		vAxis[0] = { 1,0,0 };
		vAxis[1] = { 0,1,0 };
		vAxis[2] = { 0,0,1 };
		vExtent[0] = (_max - vCenter).x;
		vExtent[1] = (_max - vCenter).y;
		vExtent[2] = (_max - vCenter).z;
	}
};