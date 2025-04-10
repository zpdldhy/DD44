#pragma once
#include "APawn.h"

class ACharacter : public APawn
{
public:
	ACharacter() = default;
	virtual ~ACharacter() = default;
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;
};

// ACharacter
// 스켈레톤 매쉬 추가
// 이동, 점프, 구르기, 공격