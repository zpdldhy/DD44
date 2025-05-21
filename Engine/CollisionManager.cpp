#include "pch.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "AActor.h"

void Collision::Init()
{
	collisionMap[{ShapeType::ST_BOX, ShapeType::ST_BOX}] = [](auto a, auto b) {
		auto boxA = static_pointer_cast<UBoxComponent>(a->GetShapeComponent());
		auto boxB = static_pointer_cast<UBoxComponent>(b->GetShapeComponent());

		// 충돌 검사 로직
		//if (Collision::CheckAABBToAABB(boxA->GetBounds(), boxB->GetBounds())) {
		//	// 충돌 처리
		//}
		boxA->CollisionAction();	// 여기서 어떤 정보를 넘겨줘야 할까?
		boxB->CollisionAction();
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
		pObj->m_bCollision = true;		// 임시
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

bool Collision::CheckRayCollision(const Ray& _ray, vector<UINT> _vActorIndex, shared_ptr<class AActor>& _pColActor, shared_ptr<UShapeComponent>& _pColShape)
{
	vector<shared_ptr<AActor>> vActorList;

	for (auto index : _vActorIndex)
		vActorList.emplace_back(OBJECT->GetActor(index));

	float dis = 999999.f;
	shared_ptr<AActor> pActor = nullptr;
	shared_ptr<UShapeComponent> hitShape = nullptr;

	for (auto& pObj : vActorList)
	{
		Vec3 inter;
		std::shared_ptr<UShapeComponent> tmpShape;

		auto pShape = pObj->GetShapeComponent();
		if (!pShape) continue;

		// Child가 없을 시
		if (pShape->GetChildren().empty())
		{
			if (!pShape || pShape->GetShapeType() == ShapeType::ST_NONE) continue;

			Ray colRay = Ray(_ray.position - pShape->GetWorldPosition(), _ray.direction);

			if (pShape->GetShapeType() == ShapeType::ST_BOX)
			{
				auto box = static_pointer_cast<UBoxComponent>(pObj->GetShapeComponent());

				if (CheckAABBToRay(colRay, box->GetBounds(), inter))
				{
					float interdis = Vec3::Distance(inter, colRay.position);
					if (dis < interdis)continue;

					dis = interdis;
					pActor = pObj;
					hitShape = pShape;
				}
			}
		}

		// Child가 있을 시
		else
			for (auto& child : pShape->GetChildren())
			{
				auto& temp = child->GetWorldPosition();

				Ray colRay = Ray(_ray.position - child->GetWorldPosition(), _ray.direction);

				if (child->GetShapeType() == ShapeType::ST_BOX)
				{
					auto box = static_pointer_cast<UBoxComponent>(pObj->GetShapeComponent());

					if (CheckAABBToRay(colRay, box->GetBounds(), inter))
					{
						float interdis = Vec3::Distance(inter, colRay.position);
						if (dis < interdis)continue;

						dis = interdis;
						pActor = pObj;
						hitShape = child;
					}
				}
			}
	}

	if (!pActor)
		return false;

	_pColActor = pActor;
	_pColShape = hitShape;

	vActorList.clear();

	return true;
}

bool Collision::CheckRayHit(const std::shared_ptr<UShapeComponent>& _shape, const Ray& _ray, Vec3& _outInter, std::shared_ptr<UShapeComponent>& _outHitShape)
{
	bool bHit = false;
	Vec3 inter;

	if (_shape->GetShapeType() == ShapeType::ST_BOX)
	{
		auto box = std::static_pointer_cast<UBoxComponent>(_shape);
		if (CheckAABBToRay(_ray, box->GetBounds(), inter))
		{
			_outInter = inter;
			_outHitShape = _shape;
			bHit = true;
		}
	}

	for (auto& child : _shape->GetChildren())
	{
		if (!child) continue;

		if (_shape->GetShapeType() == ShapeType::ST_BOX)
		{
			auto box = std::static_pointer_cast<UBoxComponent>(_shape);
			if (CheckAABBToRay(_ray, box->GetBounds(), inter))
			{
				_outInter = inter;
				_outHitShape = _shape;
				bHit = true;
			}
		}

		Vec3 tmp;
		std::shared_ptr<UShapeComponent> hitChild;

		if (CheckRayHit(child, _ray, tmp, hitChild))
		{
			_outInter = tmp;
			_outHitShape = hitChild;
			bHit = true;
		}
	}

	return bHit;
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