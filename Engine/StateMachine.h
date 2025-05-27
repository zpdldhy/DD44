#pragma once
#include "pch.h"
#include "StateBase.h"

class StateMachine
{
protected:
	map<UINT, shared_ptr<StateBase>> m_mStateMap;
public:
	void AddTransition(UINT _iTo, UINT _iEvent, UINT _iFrom);
};