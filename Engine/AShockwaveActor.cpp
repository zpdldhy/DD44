#include "pch.h"
#include "AShockwaveActor.h"

Vec3 AShockwaveActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity,float _scale)
{
    Vec3 pos = _pos;
    pos.y += 0.5f;
    SetPosition(pos);
    
    SetGravity(0.0f);
    SetStartScale(Vec3(.5f)*_scale);
    SetEndScale(Vec3(15.0f) * _scale);
    InitSpriteAnimation(1, 0.6f); 
    return Vec3(0, 0, 0); 
}
