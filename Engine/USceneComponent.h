#pragma once
#include "UActorComponent.h"

class USceneComponent : public UActorComponent
{
protected:
	Vec3 m_vLocalScale = { 1.0f, 1.0f, 1.0f };
	Vec3 m_vLocalRotation = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vLocalPosition = { 0.0f, 0.0f, 0.0f };

public:
	USceneComponent() = default;
	virtual ~USceneComponent() = default;

public:
	Vec3 GetLocalPosition() { return m_vLocalPosition; }
	Vec3 GetLocalRotation() { return m_vLocalRotation; }
	Vec3 GetLocalScale() { return m_vLocalScale; }
	void SetLocalPosition(const Vec3& _pos) { m_vLocalPosition = _pos; }
	void SetLocalRotation(const Vec3& _rot) { m_vLocalRotation = _rot; }
	void SetLocalScale(const Vec3& _scale) { m_vLocalScale = _scale; }
};

