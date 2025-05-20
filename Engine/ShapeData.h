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
	Vec3 vExtent = { 0.f, 0.f, 0.f };

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
		vExtent = _max - vCenter;

		//vVertex[0] = { vMin.x, vMax.y, vMin.z };	// front ÁÂ»ó
		//vVertex[1] = { vMax.x, vMax.y, vMin.z };	// front ¿ì»ó
		//vVertex[2] = vMin;	// front ÁÂÇÏ
		//vVertex[3] = { vMax.x, vMin.y, vMin.z };	// front ¿ìÇÏ
		//vVertex[4] = { vMin.x, vMax.y, vMax.z };	// back ÁÂ»ó
		//vVertex[5] = vMax;	// back ¿ì»ó
		//vVertex[6] = { vMin.x, vMin.y, vMax.z };	// back ÁÂÇÏ
		//vVertex[7] = { vMax.x, vMin.y, vMax.z };	// back ¿ìÇÏ
	}
};