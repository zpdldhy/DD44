#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
protected:
	Vec3 m_vCenter = Vec3::Zero;
	Vec3 m_vBoxHalfSize = Vec3::Zero;	// Scale

	shared_ptr<class AActor> m_pRange;

public:
	UBoxComponent() = default;
	virtual ~UBoxComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;

public:
	void UpdateBounds() override;

public:
	void SetBoxSize(Vec3 _vBoxHalfSize) { m_vBoxHalfSize = _vBoxHalfSize; }

	Vec3 GetBoxSize() { return m_vBoxHalfSize; }
};

