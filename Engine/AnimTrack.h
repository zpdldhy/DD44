#pragma once
#include "UAnimInstance.h"

class AnimTrack
{
public:
	shared_ptr<UAnimInstance> animInstance;
	//vector<AnimList> currentAnim;
	CbAnimData currentAnimData;
	//vector<AnimList> animTrackList;
	//vector<Matrix> inverseBone;
public:
	void Render();
	void SetBase(shared_ptr<UAnimInstance> _animInstance) { animInstance = _animInstance; }
};
