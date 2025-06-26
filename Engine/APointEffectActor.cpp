#include "pch.h"
#include "APointEffectActor.h"

Vec3 APointEffectActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity, float _scale)
{
    Vec3 pos = _pos;
    SetPosition(pos);

    SetGravity(0.0f);
    SetStartScale(Vec3(6.f) * _scale);
    SetEndScale(Vec3(.5f) * _scale);
    InitSpriteAnimation(1, 0.6f);
    return Vec3(0, 0, 0);
}
