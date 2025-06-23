#pragma once
#include "UAnimInstance.h"

class AnimTrack
{
public:
	shared_ptr<UAnimInstance> animInstance;
public:
	void SetBase(shared_ptr<UAnimInstance> _animInstance) { animInstance = _animInstance; }
};
