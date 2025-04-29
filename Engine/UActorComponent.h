#pragma once
#include "UObject.h"

class UActorComponent : public UObject
{
public:
	UActorComponent() = default;
	virtual ~UActorComponent() = default;

protected:
	class AActor* m_pOwner = nullptr;

public:
	void SetOwner(AActor* _owner) { m_pOwner = _owner; }
};

