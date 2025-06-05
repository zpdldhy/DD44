#pragma once
#include "Singleton.h"

struct pair_hash {
    template<typename T1, typename T2>
    size_t operator()(const pair<T1, T2>& p) const {
        return hash<T1>()(p.first) ^ hash<T2>()(p.second);
    }
};

class Collision : public Singleton<Collision>
{
    using CollisionFunc = function<void(shared_ptr<class AActor>, shared_ptr<class AActor>)>;
    unordered_map<pair<ShapeType, ShapeType>, CollisionFunc, pair_hash> collisionMap;

public:
    void Init();

public:
    void CheckCollision(vector<UINT> _vActorIndex);
    void CheckCollision(vector<shared_ptr<class AActor>> _vActorIist);
    bool CheckCollision(shared_ptr<class AActor> _p1, shared_ptr<class AActor> _p2);
    static bool CheckRayCollision(const Ray& _ray, vector<UINT> _vActorIndex, shared_ptr<class AActor>& _pColActor);

public:
    // Ray
    static bool CheckRayToPlane(const Ray& _ray, const Plane& _plane);
    static bool CheckMousePicking(const Ray& _ray, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2, const Vec3& _normal, Vec3& _inter);
    // Box
    static bool CheckAABBToRay(const Ray& _ray, const Box& _box, Vec3& _inter);
    static bool CheckOBBToRay(const Ray& _ray, const Box& _box, Vec3& inter);
    static bool CheckOBBToOBB(const Box& _box0, const Box& _box1);
    // Sphere
    static bool CheckSphereToSphere(const Sphere& _Sphere0, const Sphere& _Sphere1, Vec3& _normal);
    static bool CheckSphereToOBB(const Sphere& _Sphere, const Box& _Box);

    static bool GetIntersection(const Ray& _ray, const Vec3& _point, const Vec3& _normal, Vec3& _inter);
    static bool PointInPolygon(const Vec3& _inter, const Vec3& _faceNormal, const Vec3& _v0, const Vec3& _v1, const Vec3& _v2);
};

class Collision2D
{
public:
    static bool CheckRectToPoint(const POINT& _point, const Vec2& _rectMin, const Vec2& _rectMax);
};