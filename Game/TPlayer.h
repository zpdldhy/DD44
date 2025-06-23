#pragma once
#include "TCharacter.h"

class TPlayer : public TCharacter
{
public:
	virtual ~TPlayer() = default;
private:
	int arrowCount = 4;
	bool isClimbing = false;

	// ±‚≈∏ status
	int m_iSoul = 0;


public:
	int GetArrowCount() { return arrowCount; }
	void IncArrowCount(int _count = 1);
	void DecArrowCount(int _count = 1);
public:
	bool IsClimbing() { return isClimbing; }
	void StartClimbing();
	void StopClimbing();
public:
	int GetSoul() const { return m_iSoul; }
	void AddSoul(int _amount) { m_iSoul += _amount; }
	bool SpendSoul(int _amount)
	{
		int soul = m_iSoul;
		if ((soul - _amount) < 0)
		{
			return false;
		}
		else
		{
			m_iSoul -= _amount;
			return true;
		}
	}
};

