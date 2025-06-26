#pragma once

class StateBase
{
protected: 
	UINT m_iStateId = 0;
	bool m_bOnPlaying = false;
	bool m_bCanInterrupt = false;
	float m_fDissolveTimer = 0.0f;
public:
	StateBase(UINT _iStateId) { m_iStateId = _iStateId; }
	virtual ~StateBase() {};
public:
	virtual void Enter() = 0;
	virtual void Tick() = 0;
	virtual void End() = 0;
public:
	virtual bool IsPlaying() { return m_bOnPlaying; }
	virtual bool IsInterruptible() { return m_bCanInterrupt; }
	virtual UINT GetId() { return m_iStateId; }
};