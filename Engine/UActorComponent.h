#pragma once

class UActorComponent
{
protected:
	class AActor* m_pOwner = nullptr;

public:
	void SetOwner(AActor* _owner) { m_pOwner = _owner; }
};

