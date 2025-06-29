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
	void PlayEffect(EEffectType type, const Vec3& pos, float angle, const Vec3& velocity, float _size = 1, const Vec3& targetPos = Vec3(0, 0, 0));
	void PlayDustBurst(const Vec3& _origin, float _speed, float _scale);
	void PlayBeamBurst(const Vec3& origin, int count, float _scale);
	void PlayInit();

	void EffectMove();

private:
	unordered_map<EEffectType, vector<shared_ptr<AEffectActor>>> m_mEffectPool;
	array<shared_ptr<class AInstance>, static_cast<size_t>(EEffectType::Count)> m_vInstanceEffect;
	vector<shared_ptr<AEffectActor>> m_vEffectList;

	shared_ptr<AEffectActor> CreateEffectActor(EEffectType type);
	shared_ptr<AEffectActor> GetReusableActor(EEffectType type);


public:
	std::vector<std::shared_ptr<AEffectActor>> m_vBloodActorList;
	std::vector<std::shared_ptr<AEffectActor>>& GetBloodList() { return m_vBloodActorList; }

	std::vector<std::shared_ptr<AEffectActor>> m_vSoulActorList;
	std::vector<std::shared_ptr<AEffectActor>>& GetSoulList() { return m_vSoulActorList; }
};
