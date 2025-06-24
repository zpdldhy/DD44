#include "pch.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "UBoxComponent.h"
#include "USphereComponent.h"

void Collision::Init()
{
	collisionMap[{ShapeType::ST_BOX, ShapeType::ST_BOX}] = [](auto a, auto b) {
		auto boxA = static_pointer_cast<UBoxComponent>(a->GetShapeComponent());
		auto boxB = static_pointer_cast<UBoxComponent>(b->GetShapeComponent());

		//충돌 검사 로직
		if (Collision::CheckOBBToOBB(boxA->GetBounds(), boxB->GetBounds()))
		{
			// 충돌 처리
			CollisionData data;
			if (b->m_szName == L"Terrain" || b->m_szName == L"Stair")
				a->GetPhysicsComponent()->m_bColGrounded = true;
			data.otherShape = b->GetShapeComponent();

			a->m_vCollisionList.insert(make_pair(b->m_Index, data));
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
		auto Sphere = static_pointer_cast<USphereComponent>(a->GetShapeComponent());
		auto Box = static_pointer_cast<UBoxComponent>(b->GetShapeComponent());

		Vec3 Inter;
		if (Collision::CheckSphereToOBB(Sphere->GetBounds(), Box->GetBounds(), Inter))
		{
			// 충돌 처리
			CollisionData data;
			data.Inter = Inter;
			if(Collision::CheckRayToOBB(Sphere->GetGroundRay(), Box->GetBounds(), Inter))
				a->GetPhysicsComponent()->m_bColGrounded = true;
			data.otherShape = b->GetShapeComponent();

			a->m_vCollisionList.insert(make_pair(b->m_Index, data));
			b->m_vCollisionList.insert(make_pair(a->m_Index, data));
		}

		};


	collisionMap[{ShapeType::ST_SPHERE, ShapeType::ST_SPHERE}] = [](auto a, auto b) {
		auto Sphere0 = static_pointer_cast<USphereComponent>(a->GetShapeComponent());
		auto Sphere1 = static_pointer_cast<USphereComponent>(b->GetShapeComponent());

		Vec3 Inter;
		if (Collision::CheckSphereToSphere(Sphere0->GetBounds(), Sphere1->GetBounds(), Inter))
		{
			// 충돌 처리
			CollisionData data;
			data.Inter = Inter;
			if (b->m_szName == L"Terrain" || b->m_szName == L"Stair")
				a->GetPhysicsComponent()->m_bColGrounded = true;
			data.otherShape = b->GetShapeComponent();

			a->m_vCollisionList.insert(make_pair(b->m_Index, data));
		}
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

	for (auto& pSub : vActorList)
	{
		pSub->m_bCollision = true;      // 임시, Quad 구현시 제거 예정
		if (pSub->m_bCollision == false) continue;

		auto subShape = pSub->GetShapeComponent();
		if (subShape == nullptr ||
			subShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

		for (auto& pObj : vActorList)
		{
			// 벽끼리 충돌을 제외하기 위한 로직.
			// 충돌이 필요한 Object가 있으면 다른 이름으로 지어야 함.
			if (pSub->m_szName == L"Object" && pObj->m_szName == L"Object") continue;

			if (pObj->m_bCollision == false) continue;
			if (pSub == pObj) continue;
			auto objShape = pObj->GetShapeComponent();
			if (objShape == nullptr ||
				objShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

			auto subType = subShape->GetShapeType();
			auto objType = objShape->GetShapeType();

			auto key = make_pair(subType, objType);
			auto iter = collisionMap.find(key);

			if (iter != collisionMap.end())
			{
				iter->second(pSub, pObj);
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

	for (auto& pSub : vActorList)
	{
		pSub->m_bCollision = true;      // 임시, Quad 구현시 제거 예정
		if (pSub->m_bCollision == false) continue;

		auto subShape = pSub->GetShapeComponent();
		if (subShape == nullptr ||
			subShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

		for (auto& pObj : vActorList)
		{
			// 벽끼리 충돌을 제외하기 위한 로직.
			// 충돌이 필요한 Object가 있으면 다른 이름으로 지어야 함.
			if (pSub->m_szName == L"Object" && pObj->m_szName == L"Object") continue;

			if (pObj->m_bCollision == false) continue;
			if (pSub == pObj) continue;
			auto objShape = pObj->GetShapeComponent();
			if (objShape == nullptr ||
				objShape->GetCollisionType() == CollisionEnabled::CE_NOCOLLISION) continue;

			auto subType = subShape->GetShapeType();
			auto objType = objShape->GetShapeType();

			auto key = make_pair(subType, objType);
			auto iter = collisionMap.find(key);

			if (iter != collisionMap.end())
			{
				iter->second(pSub, pObj);
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

			if (CheckRayToOBB(_ray, box->GetBounds(), inter))
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

bool Collision::CheckRayToAABB(const Ray& _ray, const Box& _box, Vec3& _inter)
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

bool Collision::CheckRayToOBB(const Ray& _ray, const Box& _box, Vec3& inter)
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

int Collision::CheckAABBToPlane(const Box& _box, const Plane& _plane)
{
	Plane Plane = _plane;
	Plane.Normalize();

	Vec3 vNormal = Plane.Normal();
	vNormal.Normalize();

	float fMin = _box.vMin.Dot(vNormal);
	fMin += Plane.w;
	float fMax = _box.vMax.Dot(vNormal);
	fMax += Plane.w;

	if (fMin > 0.f && fMax > 0.f)
		return 1; // Box가 Plane의 앞에 있음
	if (fMin < 0.f && fMax < 0.f)
		return -1; // Box가 Plane의 뒤에 있음
	return 0; // Box가 Plane와 겹침
}

int Collision::CheckOBBToPlane(const Box& _box, const Plane& _plane)
{
	// 평면 노멀 단위화
	Plane plane = _plane;
	plane.Normalize();

	Vec3 normal = plane.Normal();
	normal.Normalize();

	// Projection 반지름 계산
	float radius = 0.f;
	for (int i = 0; i < 3; ++i)
	{
		float proj = fabsf(_box.vAxis[i].Dot(normal));
		radius += proj * _box.vExtent[i];
	}

	// 중심점의 평면 거리
	float distance = _box.vCenter.Dot(normal) + plane.w; // plane.w == -d

	if (distance > radius)
		return 1; // Box가 Plane의 앞에 있음
	if (distance < radius)
		return -1; // Box가 Plane의 뒤에 있음

	return 0; // Box가 Plane와 겹침
}

bool Collision::CheckOBBToOBB(const Box& _box0, const Box& _box1)
{
	Vec3 Distance = _box0.vCenter - _box1.vCenter;

	float AxisDot[3][3];
	float absAxisDot[3][3];
	float DistanceDot[3];
	float absDisDot;
	float box0Radius, box1Radius;
	float totalRadius;

	// Box0의 x축에 Box1의 모든 축을 내적
	// == Box1의 x, y, z축에 Box0의 x축을 내적
	AxisDot[0][0] = _box0.vAxis[0].Dot(_box1.vAxis[0]);
	AxisDot[0][1] = _box0.vAxis[0].Dot(_box1.vAxis[1]);
	AxisDot[0][2] = _box0.vAxis[0].Dot(_box1.vAxis[2]);

	DistanceDot[0] = _box0.vAxis[0].Dot(Distance);

	absAxisDot[0][0] = (float)fabs(AxisDot[0][0]);
	absAxisDot[0][1] = (float)fabs(AxisDot[0][1]);
	absAxisDot[0][2] = (float)fabs(AxisDot[0][2]);

	// Box0의 y축에 Box1의 모든 축을 내적
	// == Box1의 x, y, z축에 Box0의 y축을 내적
	AxisDot[1][0] = _box0.vAxis[1].Dot(_box1.vAxis[0]);
	AxisDot[1][1] = _box0.vAxis[1].Dot(_box1.vAxis[1]);
	AxisDot[1][2] = _box0.vAxis[1].Dot(_box1.vAxis[2]);

	DistanceDot[1] = _box0.vAxis[1].Dot(Distance);

	absAxisDot[1][0] = (float)fabs(AxisDot[1][0]);
	absAxisDot[1][1] = (float)fabs(AxisDot[1][1]);
	absAxisDot[1][2] = (float)fabs(AxisDot[1][2]);

	// Box0의 z축에 Box1의 모든 축을 내적
	// == Box1의 x, y, z축에 Box0의 z축을 내적
	AxisDot[2][0] = _box0.vAxis[2].Dot(_box1.vAxis[0]);
	AxisDot[2][1] = _box0.vAxis[2].Dot(_box1.vAxis[1]);
	AxisDot[2][2] = _box0.vAxis[2].Dot(_box1.vAxis[2]);

	DistanceDot[2] = _box0.vAxis[2].Dot(Distance);

	absAxisDot[2][0] = (float)fabs(AxisDot[2][0]);
	absAxisDot[2][1] = (float)fabs(AxisDot[2][1]);
	absAxisDot[2][2] = (float)fabs(AxisDot[2][2]);

	// Box0의 x, y, z와 평행한 축에
	// 두 Box를 투영하여 합한 길이와, Distance를 투영한 길이를 비교
	{
		// Box0의 x에 투영
		box1Radius = _box1.vExtent[0] * absAxisDot[0][0] + _box1.vExtent[1] * absAxisDot[0][1] + _box1.vExtent[2] * absAxisDot[0][2];
		totalRadius = _box0.vExtent[0] + box1Radius;

		absDisDot = (float)fabs(DistanceDot[0]);
		if (absDisDot > totalRadius) return false;

		// Box0의 y에 투영
		box1Radius = _box1.vExtent[0] * absAxisDot[1][0] + _box1.vExtent[1] * absAxisDot[1][1] + _box1.vExtent[2] * absAxisDot[1][2];
		totalRadius = _box0.vExtent[1] + box1Radius;

		absDisDot = (float)fabs(DistanceDot[1]);
		if (absDisDot > totalRadius) return false;

		// Box0의 z에 투영
		box1Radius = _box1.vExtent[0] * absAxisDot[2][0] + _box1.vExtent[1] * absAxisDot[2][1] + _box1.vExtent[2] * absAxisDot[2][2];
		totalRadius = _box0.vExtent[2] + box1Radius;

		absDisDot = (float)fabs(DistanceDot[2]);
		if (absDisDot > totalRadius) return false;
	}

	// Box1의 x, y, z와 평행한 축에
	// 두 Box를 투영하여 합한 길이와, Distance를 투영한 길이를 비교
	{
		// Box1의 x에 투영
		box0Radius = _box0.vExtent[0] * absAxisDot[0][0] + _box0.vExtent[1] * absAxisDot[1][0] + _box0.vExtent[2] * absAxisDot[2][0];
		totalRadius = box0Radius + _box1.vExtent[0];

		absDisDot = fabs(_box1.vAxis[0].Dot(Distance));
		if (absDisDot > totalRadius)return false;

		// Box1의 y에 투영
		box0Radius = _box0.vExtent[0] * absAxisDot[0][1] + _box0.vExtent[1] * absAxisDot[1][1] + _box0.vExtent[2] * absAxisDot[2][1];
		totalRadius = box0Radius + _box1.vExtent[1];

		absDisDot = fabs(_box1.vAxis[1].Dot(Distance));
		if (absDisDot > totalRadius)return false;

		// Box1의 z에 투영
		box0Radius = _box0.vExtent[0] * absAxisDot[0][2] + _box0.vExtent[1] * absAxisDot[1][2] + _box0.vExtent[2] * absAxisDot[2][2];
		totalRadius = box0Radius + _box1.vExtent[2];

		absDisDot = fabs(_box1.vAxis[2].Dot(Distance));
		if (absDisDot > totalRadius)return false;
	}

	// 두 Box의 각 축에 외적하여 나오는 축과 비교
	{
		// Box0의 x축과 Box1의 모든 축을 외적한 벡터(각 축에 수직)에 투영
	
		//A0xB0
		box0Radius = _box0.vExtent[1] * absAxisDot[2][0] + _box0.vExtent[2] * absAxisDot[1][0];
		box1Radius = _box1.vExtent[1] * absAxisDot[0][2] + _box1.vExtent[2] * absAxisDot[0][1];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[2] * AxisDot[1][0] - DistanceDot[1] * AxisDot[2][0]);
		if (absDisDot > totalRadius)return false;

		//A0xB1
		box0Radius = _box0.vExtent[1] * absAxisDot[2][1] + _box0.vExtent[2] * absAxisDot[1][1];
		box1Radius = _box1.vExtent[0] * absAxisDot[0][2] + _box1.vExtent[2] * absAxisDot[0][0];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[2] * AxisDot[1][1] - DistanceDot[1] * AxisDot[2][1]);
		if (absDisDot > totalRadius)return false;

		//A0xB2
		box0Radius = _box0.vExtent[1] * absAxisDot[2][2] + _box0.vExtent[2] * absAxisDot[1][2];
		box1Radius = _box1.vExtent[0] * absAxisDot[0][1] + _box1.vExtent[1] * absAxisDot[0][0];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[2] * AxisDot[1][2] - DistanceDot[1] * AxisDot[2][2]);
		if (absDisDot > totalRadius)return false;

		//A1xB0
		box0Radius = _box0.vExtent[0] * absAxisDot[2][0] + _box0.vExtent[2] * absAxisDot[0][0];
		box1Radius = _box1.vExtent[1] * absAxisDot[1][2] + _box1.vExtent[2] * absAxisDot[1][1];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[0] * AxisDot[2][0] - DistanceDot[2] * AxisDot[0][0]);
		if (absDisDot > totalRadius)return false;

		//A1xB1
		box0Radius = _box0.vExtent[0] * absAxisDot[2][1] + _box0.vExtent[2] * absAxisDot[0][1];
		box1Radius = _box1.vExtent[0] * absAxisDot[1][2] + _box1.vExtent[2] * absAxisDot[1][0];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[0] * AxisDot[2][1] - DistanceDot[2] * AxisDot[0][1]);
		if (absDisDot > totalRadius)return false;

		//A1xB2
		box0Radius = _box0.vExtent[0] * absAxisDot[2][2] + _box0.vExtent[2] * absAxisDot[0][2];
		box1Radius = _box1.vExtent[0] * absAxisDot[1][1] + _box1.vExtent[1] * absAxisDot[1][0];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[0] * AxisDot[2][2] - DistanceDot[2] * AxisDot[0][2]);
		if (absDisDot > totalRadius)return false;

		//A2xB0
		box0Radius = _box0.vExtent[0] * absAxisDot[1][0] + _box0.vExtent[1] * absAxisDot[0][0];
		box1Radius = _box1.vExtent[1] * absAxisDot[2][2] + _box1.vExtent[2] * absAxisDot[2][1];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[1] * AxisDot[0][0] - DistanceDot[0] * AxisDot[1][0]);
		if (absDisDot > totalRadius)return false;

		//A2xB1
		box0Radius = _box0.vExtent[0] * absAxisDot[1][1] + _box0.vExtent[1] * absAxisDot[0][1];
		box1Radius = _box1.vExtent[0] * absAxisDot[2][2] + _box1.vExtent[2] * absAxisDot[2][0];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[1] * AxisDot[0][1] - DistanceDot[0] * AxisDot[1][1]);
		if (absDisDot > totalRadius)return false;

		//A2xB2
		box0Radius = _box0.vExtent[0] * absAxisDot[1][2] + _box0.vExtent[1] * absAxisDot[0][2];
		box1Radius = _box1.vExtent[0] * absAxisDot[2][1] + _box1.vExtent[1] * absAxisDot[2][0];
		totalRadius = box0Radius + box1Radius;
		absDisDot = fabs(DistanceDot[1] * AxisDot[0][2] - DistanceDot[0] * AxisDot[1][2]);
		if (absDisDot > totalRadius)return false;
	}

	return true;
}

bool Collision::CheckSphereToSphere(const Sphere& _Sphere0, const Sphere& _Sphere1, Vec3& _inter)
{
	float Dis = (_Sphere0.vCenter - _Sphere1.vCenter).Length();
	float totalRadius = _Sphere0.fRadius + _Sphere1.fRadius;

	if (Dis > totalRadius)
		return false;

	// normal은 Sphere0 기준의 normal 벡터
	auto normal = _Sphere0.vCenter - _Sphere1.vCenter;
	normal.Normalize();

	_inter = _Sphere1.vCenter + normal * _Sphere1.fRadius;

	return true;
}

bool Collision::CheckSphereToOBB(const Sphere& _Sphere, const Box& _Box, Vec3& _Inter)
{
	Vec3 Dir = _Sphere.vCenter - _Box.vCenter;
	Vec3 closePoint = _Box.vCenter;

	for (int i = 0; i < 3; i++)
	{
		float D = (Dir).Dot(_Box.vAxis[i]);
		float Clamp = max(-_Box.vExtent[i], min(_Box.vExtent[i], D));

		closePoint = closePoint + _Box.vAxis[i] * Clamp;
	}

	Vec3 diff = _Sphere.vCenter - closePoint;

	if (diff.Length() > _Sphere.fRadius)
		return false;

	_Inter = closePoint;
	return true;
}

bool Collision::GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& _inter)
{
	float dot1 = _normal.Dot(_ray.direction);
	Vec3 temp = _point - _ray.position;
	float dot2 = _normal.Dot(temp);

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
