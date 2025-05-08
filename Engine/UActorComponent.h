#pragma once
#include "UObject.h"

class UActorComponent : public UObject
{
public:
	UActorComponent() = default;
	virtual ~UActorComponent() = default;

protected:
	weak_ptr<class AActor> m_pOwner;

public:
	void SetOwner(shared_ptr<class AActor> _pOwner) { m_pOwner = _pOwner; }
	shared_ptr<class AActor> GetOwner() { return m_pOwner.lock(); }
};

