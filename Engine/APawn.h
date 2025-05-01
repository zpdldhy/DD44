#pragma once
#include "AActor.h"

class APawn : public AActor
{
public:
	APawn() = default;
	virtual ~APawn() = default;
};

// APawn
// 플레이어나 AI가 조종 가능한 Actor
// 입력, 컨트롤