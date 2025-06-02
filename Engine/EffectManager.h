#pragma once
#include "Singleton.h"
#include "EffectType.h"
#include "AEffectActor.h"
#include <unordered_map>

class EffectManager : public Singleton<EffectManager>
{
public:
	void Init();          
	void PlayEffect(EEffectType type, const Vec3& pos, float angle, const Vec3& velocity);

private:
	unordered_map<EEffectType, vector<shared_ptr<AEffectActor>>> m_mEffectPool;

	shared_ptr<AEffectActor> CreateEffectActor(EEffectType type);
	shared_ptr<AEffectActor> GetReusableActor(EEffectType type);

};
