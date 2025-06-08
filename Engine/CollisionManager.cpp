#include "pch.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "UBoxComponent.h"

float LengthSq(Vec3 _v)
{
	return _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
}

Vec3 GetNormalized(Vec3 _v)
{
	float len = sqrtf(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z);
	if (len == 0.0f) return Vec3(0, 0, 0); // 0벡터 방어
	return Vec3(_v.x / len, _v.y / len, _v.z / len);
}


void Collision::Init()
{
	collisionMap[{ShapeType::ST_BOX, ShapeType::ST_BOX}] = [](auto a, auto b) {
		auto boxA = static_pointer_cast<UBoxComponent>(a->GetShapeComponent());
		auto boxB = static_pointer_cast<UBoxComponent>(b->GetShapeComponent());

		Vec3 output;
		//충돌 검사 로직
		if (Collision::CheckOBBToOBB(boxA->GetBounds(), boxB->GetBounds(), &output))
		{
			// 충돌 처리
			CollisionData data;
			data.box = boxB->GetBounds();
			data.ColNormal = output;
			if (b->m_szName == L"Terrain"|| b->m_szName == L"Stair")
				data.bColGround = true;

			boxA->AddCollision(b->m_Index, data);
		}
		};

	collisionMap[{ShapeType::ST_BOX, ShapeType::ST_SPHERE}] = [](auto a, auto b) {
		auto boxA = static_pointer_cast<UBoxComponent>(a->GetShapeComponent());
		// ...
		};

	collisionMap[{ShapeType::ST_BOX, ShapeType::ST_CAPSULE}] = [](auto a, auto b) {
		auto boxA = static_pointer_cast<UBoxComponent>(a->GetShapeComponent());
		// ...
		};

	collisionMap[{ShapeType::ST_SPHERE, ShapeType::ST_BOX}] = [](auto a, auto b) {
		// ...
		};

	collisionMap[{ShapeType::ST_SPHERE, ShapeType::ST_SPHERE}] = [](auto a, auto b) {
		// ...
		};

	collisionMap[{ShapeType::ST_SPHERE, ShapeType::ST_CAPSULE}] = [](auto a, auto b) {
		// ...
		};

	collisionMap[{ShapeType::ST_CAPSULE, ShapeType::ST_BOX}] = [](auto a, auto b) {
		// ...
		};

	collisionMap[{ShapeType::ST_CAPSULE, ShapeType::ST_SPHERE}] = [](auto a, auto b) {
		// ...
		};

	collisionMap[{ShapeType::ST_CAPSULE, ShapeType::ST_CAPSULE}] = [](auto a, auto b) {
		// ...
		};
}

void Collision::CheckCollision(vector<UINT> _vActorIndex)
{
	// 현재 Quad에 아무 Object가 없을 때 바로 종료
	if (_vActorIndex.empty())
		return;

	// Collision이 있는 Actor의 List를 가져온다.
	vector<shared_ptr<AActor>> vActorList;

	for (auto index : _vActorIndex)
		vActorList.emplace_back(OBJECT->GetActor(index));

	for (auto& pObj : vActorList)
	{
		pObj->m_bCollision = true;      // 임시
		if (pObj->m_bCollision == false) continue;

		auto objShape = pObj->GetShapeComponent();
		if (objShape == nullptr ||
			objShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

		for (auto& pSub : vActorList)
		{
			// 임시?
			if (pObj->m_szName == L"Object" && pSub->m_szName == L"Object") continue;

			if (pSub->m_bCollision == false) continue;
			if (pObj == pSub) continue;
			auto subShape = pSub->GetShapeComponent();
			if (subShape == nullptr ||
				subShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

			auto objType = objShape->GetShapeType();
			auto subType = subShape->GetShapeType();

			auto key = make_pair(objType, subType);
			auto iter = collisionMap.find(key);

			if (iter != collisionMap.end())
			{
				iter->second(pObj, pSub);
			}
		}
	}

	vActorList.clear();
}

void Collision::CheckCollision(vector<shared_ptr<class AActor>> _vActorIist)
{
	// 현재 Quad에 아무 Object가 없을 때 바로 종료
	if (_vActorIist.empty())
		return;

	// Collision이 있는 Actor의 List를 가져온다.
	vector<shared_ptr<AActor>> vActorList = _vActorIist;

	for (auto& pObj : vActorList)
	{
		pObj->m_bCollision = true;      // 임시
		if (pObj->m_bCollision == false) continue;

		auto objShape = pObj->GetShapeComponent();
		if (objShape == nullptr ||
			objShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

		for (auto& pSub : vActorList)
		{
			if (pSub->m_bCollision == false) continue;
			if (pObj == pSub) continue;
			auto subShape = pSub->GetShapeComponent();
			if (subShape == nullptr ||
				subShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

			auto objType = objShape->GetShapeType();
			auto subType = subShape->GetShapeType();

			auto key = make_pair(objType, subType);
			auto iter = collisionMap.find(key);

			if (iter != collisionMap.end())
			{
				iter->second(pObj, pSub);
			}
		}
	}

	vActorList.clear();
}

bool Collision::CheckCollision(shared_ptr<class AActor> _p1, shared_ptr<class AActor> _p2)
{
	if (_p1 == _p2) return false;

	//_p1->m_bCollision = true;      // 임시
	//_p2->m_bCollision = true;

	if (_p1->m_bCollision == false) return false;
	if (_p2->m_bCollision == false) return false;

	auto Shape1 = _p1->GetShapeComponent();
	if (Shape1 == nullptr ||
		Shape1->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) return false;

	auto Shape2 = _p2->GetShapeComponent();
	if (Shape2 == nullptr ||
		Shape2->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) return false;

	auto Type1 = Shape1->GetShapeType();
	auto Type2 = Shape2->GetShapeType();

	auto key = make_pair(Type1, Type2);
	auto iter = collisionMap.find(key);

	if (iter != collisionMap.end())
	{
		iter->second(_p1, _p2);
	}

	return false;
}

bool Collision::CheckRayCollision(const Ray& _ray, vector<UINT> _vActorIndex, shared_ptr<class AActor>& _pColActor)
{
	// Collision이 있는 Actor의 List를 가져온다.
	vector<shared_ptr<AActor>> vActorList;

	for (auto index : _vActorIndex)
	{
		auto actor = OBJECT->GetActor(index);
		if (actor)
		{
			vActorList.emplace_back(actor);
		}
	}

	float dis = 999999.f;
	shared_ptr<AActor> pActor = nullptr;

	for (auto& pObj : vActorList)
	{
		Vec3 inter;
		auto pShape = pObj->GetShapeComponent();

		if (!pShape || pShape->GetShapeType() == ShapeType::ST_NONE) continue;

		if (pShape->GetShapeType() == ShapeType::ST_BOX)
		{
			auto box = static_pointer_cast<UBoxComponent>(pObj->GetShapeComponent());

			if (CheckOBBToRay(_ray, box->GetBounds(), inter))
			{
				float interdis = Vec3::Distance(inter, _ray.position);
				if (dis < interdis)continue;

				dis = interdis;
				pActor = pObj;
			}
		}

		else if (pShape->GetShapeType() == ShapeType::ST_SPHERE)
		{
			int i = 0;
		}
		else if (pShape->GetShapeType() == ShapeType::ST_CAPSULE)
		{
			int i = 0;
		}
	}

	if (pActor == nullptr)
		return false;

	_pColActor = pActor;

	vActorList.clear();

	return true;
}

bool Collision::CheckRayToPlane(const Ray& _ray, const Plane& _plane)
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

bool Collision::CheckMousePicking(const Ray& _ray, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2, const Vec3& _normal, Vec3& _inter)
{
	if (GetIntersection(_ray, _v0, _normal, _inter) == false)
		return false;

	return PointInPolygon(_inter, _normal, _v0, _v1, _v2);
}

bool Collision::CheckAABBToRay(const Ray& _ray, const Box& _box, Vec3& _inter)
{
	float fMin = 0.f;
	float fMax = 999999.f;

	// x축 방향이 없음 -> yz 평면
	if (abs(_ray.direction.x) < 0.0001f)
	{
		if (_ray.position.x < _box.vMin.x || _ray.position.x > _box.vMax.x)
			return false;
	}
	// x축 방향이 있음
	else
	{
		float denom = 1.f / _ray.direction.x;
		float dx1 = (_box.vMin.x - _ray.position.x) * denom;
		float dx2 = (_box.vMax.x - _ray.position.x) * denom;

		if (dx1 > dx2)
			swap(dx1, dx2);

		fMin = max(fMin, dx1);
		fMax = min(fMax, dx2);

		if (fMin > fMax)
			return false;
	}

	// y축
	if (abs(_ray.direction.y) < 0.0001f)
	{
		if (_ray.position.y < _box.vMin.y || _ray.position.y > _box.vMax.y)
			return false;
	}
	else
	{
		float denom = 1.f / _ray.direction.y;
		float dx1 = (_box.vMin.y - _ray.position.y) * denom;
		float dx2 = (_box.vMax.y - _ray.position.y) * denom;

		if (dx1 > dx2)
			swap(dx1, dx2);

		fMin = max(fMin, dx1);
		fMax = min(fMax, dx2);

		if (fMin > fMax)
			return false;
	}

	// z축
	if (abs(_ray.direction.z) < 0.0001f)
	{
		if (_ray.position.z < _box.vMin.z || _ray.position.z > _box.vMax.z)
			return false;
	}
	else
	{
		float denom = 1.f / _ray.direction.z;
		float dx1 = (_box.vMin.z - _ray.position.z) * denom;
		float dx2 = (_box.vMax.z - _ray.position.z) * denom;

		if (dx1 > dx2)
			swap(dx1, dx2);

		fMin = max(fMin, dx1);
		fMax = min(fMax, dx2);

		if (fMin > fMax)
			return false;
	}

	_inter = _ray.position + _ray.direction * fMin;

	return true;
}

bool Collision::CheckOBBToRay(const Ray& _ray, const Box& _box, Vec3& inter)
{
	float fMin = -999999.f;
	float fMax = +999999.f;

	Vec3 diff = _box.vCenter - _ray.position;

	// box의 x상대축

	for (int i = 0; i < 3; i++)
	{
		float fDotDir = _box.vAxis[i].Dot(_ray.direction);
		float fDotDiff = _box.vAxis[i].Dot(diff);

		if (abs(fDotDir) < 0.0001f)
		{
			if (fabs(fDotDiff) > _box.vExtent[i])
				return false;
		}
		else
		{
			float ret1 = (fDotDiff - _box.vExtent[i]) / fDotDir;
			float ret2 = (fDotDiff + _box.vExtent[i]) / fDotDir;

			if (ret1 > ret2)
				swap(ret1, ret2);

			fMin = max(fMin, ret1);
			fMax = min(fMax, ret2);

			if (fMin > fMax)
				return false;
		}
	}

	inter = _ray.position + _ray.direction * fMin;

	return true;
}

//bool Collision::CheckOBBToOBB(const Box& _box1, const Box& _box2, Vec3 _outNormal)
//{
//	Vec3 Distance = _box1.vCenter - _box2.vCenter;
//
//	float Dot[3][3];
//	float absDot[3][3];
//	float AD[3];
//	float result;
//	float R0, R1;
//	float R01;
//
//	float minPenetration = FLT_MAX;
//
//	// 모든 축 검사 전용 람다 (C++11 이상 가능)
//	auto TryAxis = [&](const Vec3& axis, float distProjection, float r0, float r1)
//		{
//			float overlap = r0 + r1 - fabs(distProjection);
//			if (overlap < 0.0f)
//				return false; // 분리축 발견 = 충돌 아님
//
//			if (overlap < minPenetration)
//			{
//				minPenetration = overlap;
//				_outNormal = axis * ((distProjection < 0.0f) ? -1.0f : 1.0f); // 정방향 유지
//			}
//			return true;
//		};
//
//	// 1. A의 축 3개
//	for (int i = 0; i < 3; ++i)
//	{
//		R1 = _box2.vExtent.x * absDot[i][0] + _box2.vExtent.y * absDot[i][1] + _box2.vExtent.z * absDot[i][2];
//		if (!TryAxis(_box1.vAxis[i], AD[i], _box1.vExtent[i], R1)) return false;
//	}
//
//	//A0
//	Dot[0][0] = _box1.vAxis[0].Dot(_box2.vAxis[0]);
//	Dot[0][1] = _box1.vAxis[0].Dot(_box2.vAxis[1]);
//	Dot[0][2] = _box1.vAxis[0].Dot(_box2.vAxis[2]);
//
//	AD[0] = _box1.vAxis[0].Dot(Distance);
//
//	absDot[0][0] = (float)fabs(Dot[0][0]);
//	absDot[0][1] = (float)fabs(Dot[0][1]);
//	absDot[0][2] = (float)fabs(Dot[0][2]);
//
//	result = (float)fabs(AD[0]);
//
//	R1 = _box2.vExtent.x * absDot[0][0] + _box2.vExtent.y * absDot[0][1] + _box2.vExtent.z * absDot[0][2];
//	R01 = _box1.vExtent.x + R1;
//	if (result > R01)return false;
//
//	//A1
//	Dot[1][0] = _box1.vAxis[1].Dot(_box2.vAxis[0]);
//	Dot[1][1] = _box1.vAxis[1].Dot(_box2.vAxis[1]);
//	Dot[1][2] = _box1.vAxis[1].Dot(_box2.vAxis[2]);
//
//	AD[1] = _box1.vAxis[1].Dot(Distance);
//
//	absDot[1][0] = (float)fabs(Dot[1][0]);
//	absDot[1][1] = (float)fabs(Dot[1][1]);
//	absDot[1][2] = (float)fabs(Dot[1][2]);
//
//	result = (float)fabs(AD[1]);
//
//	R1 = _box2.vExtent.x * absDot[1][0] + _box2.vExtent.y * absDot[1][1] + _box2.vExtent.z * absDot[1][2];
//	R01 = _box1.vExtent.y + R1;
//	if (result > R01)return false;
//
//	//A2
//	Dot[2][0] = _box1.vAxis[2].Dot(_box2.vAxis[0]);
//	Dot[2][1] = _box1.vAxis[2].Dot(_box2.vAxis[1]);
//	Dot[2][2] = _box1.vAxis[2].Dot(_box2.vAxis[2]);
//
//	AD[2] = _box1.vAxis[2].Dot(Distance);
//
//	absDot[2][0] = (float)fabs(Dot[2][0]);
//	absDot[2][1] = (float)fabs(Dot[2][1]);
//	absDot[2][2] = (float)fabs(Dot[2][2]);
//
//	result = (float)fabs(AD[2]);
//
//	R1 = _box2.vExtent.x * absDot[2][0] + _box2.vExtent.y * absDot[2][1] + _box2.vExtent.z * absDot[2][2];
//	R01 = _box1.vExtent.z + R1;
//	if (result > R01)return false;
//
//	//B0
//	result = fabs(_box2.vAxis[0].Dot(Distance));
//	R0 = _box1.vExtent.x * absDot[0][0] + _box1.vExtent.y * absDot[1][0] + _box1.vExtent.z * absDot[2][0];
//	R01 = R0 + _box2.vExtent.x;
//	if (result > R01)return false;
//
//	//B1
//	result = fabs(_box2.vAxis[1].Dot(Distance));
//	R0 = _box1.vExtent.x * absDot[0][1] + _box1.vExtent.y * absDot[1][1] + _box1.vExtent.z * absDot[2][1];
//	R01 = R0 + _box2.vExtent.y;
//	if (result > R01)return false;
//
//	//B2
//	result = fabs(_box2.vAxis[2].Dot(Distance));
//	R0 = _box1.vExtent.x * absDot[0][2] + _box1.vExtent.y * absDot[1][2] + _box1.vExtent.z * absDot[2][2];
//	R01 = R0 + _box2.vExtent.z;
//	if (result > R01)return false;
//
//	//A0xB0
//	result = fabs(AD[2] * Dot[1][0] - AD[1] * Dot[2][0]);
//	R0 = _box1.vExtent.y * absDot[2][0] + _box1.vExtent.z * absDot[1][0];
//	R1 = _box2.vExtent.y * absDot[0][2] + _box2.vExtent.z * absDot[0][1];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A0xB1
//	result = fabs(AD[2] * Dot[1][1] - AD[1] * Dot[2][1]);
//	R0 = _box1.vExtent.y * absDot[2][1] + _box1.vExtent.z * absDot[1][1];
//	R1 = _box2.vExtent.x * absDot[0][2] + _box2.vExtent.z * absDot[0][0];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A0xB2
//	result = fabs(AD[2] * Dot[1][2] - AD[1] * Dot[2][2]);
//	R0 = _box1.vExtent.y * absDot[2][2] + _box1.vExtent.z * absDot[1][2];
//	R1 = _box2.vExtent.x * absDot[0][1] + _box2.vExtent.y * absDot[0][0];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A1xB0
//	result = fabs(AD[0] * Dot[2][0] - AD[2] * Dot[0][0]);
//	R0 = _box1.vExtent.x * absDot[2][0] + _box1.vExtent.z * absDot[0][0];
//	R1 = _box2.vExtent.y * absDot[1][2] + _box2.vExtent.z * absDot[1][1];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A1xB1
//	result = fabs(AD[0] * Dot[2][1] - AD[2] * Dot[0][1]);
//	R0 = _box1.vExtent.x * absDot[2][1] + _box1.vExtent.z * absDot[0][1];
//	R1 = _box2.vExtent.x * absDot[1][2] + _box2.vExtent.z * absDot[1][0];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A1xB2
//	result = fabs(AD[0] * Dot[2][2] - AD[2] * Dot[0][2]);
//	R0 = _box1.vExtent.x * absDot[2][2] + _box1.vExtent.z * absDot[0][2];
//	R1 = _box2.vExtent.x * absDot[1][1] + _box2.vExtent.y * absDot[1][0];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A2xB0
//	result = fabs(AD[1] * Dot[0][0] - AD[0] * Dot[1][0]);
//	R0 = _box1.vExtent.x * absDot[1][0] + _box1.vExtent.y * absDot[0][0];
//	R1 = _box2.vExtent.y * absDot[2][2] + _box2.vExtent.z * absDot[2][1];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A2xB1
//	result = fabs(AD[1] * Dot[0][1] - AD[0] * Dot[1][1]);
//	R0 = _box1.vExtent.x * absDot[1][1] + _box1.vExtent.y * absDot[0][1];
//	R1 = _box2.vExtent.x * absDot[2][2] + _box2.vExtent.z * absDot[2][0];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	//A2xB2
//	result = fabs(AD[1] * Dot[0][2] - AD[0] * Dot[1][2]);
//	R0 = _box1.vExtent.x * absDot[1][2] + _box1.vExtent.y * absDot[0][2];
//	R1 = _box2.vExtent.x * absDot[2][1] + _box2.vExtent.y * absDot[2][0];
//	R01 = R0 + R1;
//	if (result > R01)return false;
//
//	return true;
//}

bool Collision::CheckOBBToOBB(const Box& _box1, const Box& _box2, Vec3* _outNormal)
{
	Vec3 Distance = _box1.vCenter - _box2.vCenter;

	float Dot[3][3];
	float absDot[3][3];
	float AD[3];
	float result;
	float R0, R1;
	float R01;

	float minPenetration = FLT_MAX;
	Vec3 bestAxis;

	// 모든 축 검사 전용 람다 (C++11 이상 가능)
	auto TryAxis = [&](const Vec3& axis, float distProjection, float r0, float r1)
		{
			float overlap = r0 + r1 - fabs(distProjection);
			if (overlap < 0.0f)
				return false; // 분리축 발견 = 충돌 아님

			if (overlap < minPenetration)
			{
				minPenetration = overlap;
				bestAxis = axis * ((distProjection < 0.0f) ? -1.0f : 1.0f); // 정방향 유지
			}
			return true;
		};

	// Dot, absDot, AD 계산
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Dot[i][j] = _box1.vAxis[i].Dot(_box2.vAxis[j]);
			absDot[i][j] = fabs(Dot[i][j]);
		}
		AD[i] = _box1.vAxis[i].Dot(Distance);
	}

	// 1. A의 축 3개
	for (int i = 0; i < 3; ++i)
	{
		R1 = _box2.vExtent[0] * absDot[i][0] + _box2.vExtent[1] * absDot[i][1] + _box2.vExtent[2] * absDot[i][2];
		if (!TryAxis(_box1.vAxis[i], AD[i], _box1.vExtent[i], R1)) return false;
	}

	// 2. B의 축 3개
	for (int i = 0; i < 3; ++i)
	{
		float proj = _box2.vAxis[i].Dot(Distance);
		R0 = _box1.vExtent[0] * absDot[0][i] + _box1.vExtent[1] * absDot[1][i] + _box1.vExtent[2] * absDot[2][i];
		if (!TryAxis(_box2.vAxis[i], proj, R0, _box2.vExtent[i])) return false;
	}

	// 3. 교차 축 A_i x B_j (9개)
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vec3 axis = _box1.vAxis[i].Cross(_box2.vAxis[j]);
			if (LengthSq(axis) < 1e-6f) continue; // 거의 평행

			float proj = Distance.Dot(axis);

			R0 = _box1.vExtent[(i + 1) % 3] * fabs(_box1.vAxis[(i + 2) % 3].Dot(_box2.vAxis[j])) +
				_box1.vExtent[(i + 2) % 3] * fabs(_box1.vAxis[(i + 1) % 3].Dot(_box2.vAxis[j]));

			R1 = _box2.vExtent[(j + 1) % 3] * fabs(_box2.vAxis[(j + 2) % 3].Dot(_box1.vAxis[i])) +
				_box2.vExtent[(j + 2) % 3] * fabs(_box2.vAxis[(j + 1) % 3].Dot(_box1.vAxis[i]));

			axis.Normalize();
			if (!TryAxis(axis, proj, R0, R1)) return false;
		}
	}

	if (_outNormal) *_outNormal = bestAxis;
	return true;
}

bool Collision::GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& _inter)
{
	float dot1 = _normal.Dot(_ray.direction);
	float dot2 = _normal.Dot(_point - _ray.position);

	float Dt = dot2 / dot1;

	if (Dt < 0 || Dt>1)
		return false;

	_inter = _ray.position + _ray.direction * Dt;
	return true;
}

bool Collision::PointInPolygon(const Vec3& _inter, const Vec3& _faceNormal, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2)
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

// 화면좌표계 충돌
bool Collision2D::CheckRectToPoint(const POINT& _point, const Vec2& _rectMin, const Vec2& _rectMax)
{
	if (_point.x < _rectMin.x)
		return false;
	if (_point.x > _rectMax.x)
		return false;
	if (_point.y < _rectMin.y)
		return false;
	if (_point.y > _rectMax.y)
		return false;

	return true;
}
