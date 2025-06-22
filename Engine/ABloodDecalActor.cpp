#include "pch.h"
#include "ABloodDecalActor.h"

Vec3 ABloodDecalActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity, float _scale)
{
    Vec3 pos = _pos;
    float size = RandomRange(1.5, 3);
        
    SetGravity(0.0f);
    SetPosition(pos);
    SetStartScale(Vec3(size) * _scale);
    SetEndScale(Vec3(size) * _scale);
    InitSpriteAnimation(1, 0.6f);
    return Vec3(0, 0, 0);
}
