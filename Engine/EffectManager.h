#pragma once
#include "Singleton.h"
#include "EffectType.h"
#include "AEffectActor.h"
#include <unordered_map>

class EffectManager : public Singleton<EffectManager>
{
public:
	void Init();          
	void Tick();
	void Render();

public:
	void PlayEffect(EEffectType type, const Vec3& pos, float angle, const Vec3& velocity, float _size = 1);
	void PlayDustBurst(const Vec3& _origin, float _speed, float _scale);
	void PlayBeamBurst(const Vec3& origin, int count, float _scale);

private:
	unordered_map<EEffectType, vector<shared_ptr<AEffectActor>>> m_mEffectPool;
	vector<shared_ptr<AEffectActor>> m_vEffectList;

	shared_ptr<AEffectActor> CreateEffectActor(EEffectType type);
	shared_ptr<AEffectActor> GetReusableActor(EEffectType type);

};
