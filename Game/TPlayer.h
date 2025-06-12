#pragma once
#include "TCharacter.h"

class TPlayer : public TCharacter
{
public:
	virtual ~TPlayer() = default;
private:
	int arrowCount = 4;

	// ±‚≈∏ status
	//int 
public:
	int GetArrowCount() { return arrowCount; }
	void IncArrowCount(int _count = 1);
	void DecArrowCount(int _count = 1);
};

