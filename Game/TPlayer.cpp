#include "pch.h"
#include "TPlayer.h"
#include <algorithm>

void TPlayer::IncArrowCount(int _count)
{
	arrowCount += _count;
	// �ִ� 4
	arrowCount = arrowCount >= 4 ? 4 : arrowCount; 
}

void TPlayer::DecArrowCount(int _count)
{
	arrowCount -= _count;
	// �ּڰ� 0
	arrowCount = arrowCount <= 0 ? 0 : arrowCount;
}
