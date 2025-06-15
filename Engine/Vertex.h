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

struct IW_VERTEX
{
	float	i1[4], i2[4];
	float	w1[4], w2[4];
	IW_VERTEX()
	{
		i1[3] = i1[2] = i1[1] = i1[0] = 0.0f;
		w1[3] = w1[2] = w1[1] = w1[0] = 0.0f;
		i2[3] = i2[2] = i2[1] = i2[0] = 0.0f;
		w2[3] = w2[2] = w2[1] = w2[0] = 0.0f;
	}
};

struct INSTANCE_VERTEX
{
	Matrix matWorld;
};