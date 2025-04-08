#pragma once
struct PNCT_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec4 color;
	Vec2 uv;
	PNCT_VERTEX() {};
	PNCT_VERTEX(Vec3 _p, Vec3 _n, Vec4 _c, Vec2 _t)
	{
		pos = _p;
		normal = _n;
		color = _c;
		uv = _t;
	}
};