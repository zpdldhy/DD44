#pragma once
#include "Singleton.h"
#include "EffectType.h"
#include "AEffectActor.h"
#include <unordered_map>

class EffectManager : public Singleton<EffectManager>
{
public:
    void Init();            // �ʱ� Pool ����
    void PlayEffect(EEffectType type, const Vec3& pos, float angle, const Vec3& velocity);

private:
    unordered_map<EEffectType, vector<shared_ptr<AEffectActor>>> m_mEffectPool;

    shared_ptr<AEffectActor> CreateEffectActor(EEffectType type);
    shared_ptr<AEffectActor> GetReusableActor(EEffectType type);

    void MakeBillboardQuadVerts(
        const Vec3& center,
        const Vec2& size,
        const Vec3& cameraPos,
        std::array<Vec3, 4>& outVerts,
        std::array<Vec2, 4>& outUVs
    );
};
