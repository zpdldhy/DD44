#pragma once
#include "UScriptComponent.h"

class AActor;

class GameCameraMove : public UScriptComponent
{
private:
	shared_ptr<const AActor> m_pTarget;

	Vec3 m_vOffset;
public:
	GameCameraMove(const shared_ptr<AActor>& _pTarget) : m_pTarget(_pTarget) {};
	virtual ~GameCameraMove() = default;

public:
	void Init() override;
	void Tick() override;
};

