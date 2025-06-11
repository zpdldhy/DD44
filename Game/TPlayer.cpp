#include "pch.h"
#include "TPlayer.h"
#include <algorithm>

void TPlayer::IncArrowCount(int _count)
{
	arrowCount += _count;
	// ÃÖ´ñ°ª 4
	arrowCount = arrowCount >= 4 ? 4 : arrowCount; 
}

void TPlayer::DecArrowCount(int _count)
{
	arrowCount -= _count;
	// ÃÖ¼Ú°ª 0
	arrowCount = arrowCount <= 0 ? 0 : arrowCount;
}
