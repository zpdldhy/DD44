#pragma once
#include "AActor.h"
#include "ULightComponent.h"

class ALight : public AActor
{
public:
    ALight();
    virtual ~ALight() = default;

    std::shared_ptr<ULightComponent> GetLightComponent() const { return m_pLightComponent; }

public:
    void UpdateLightLookAt(Vec3 _vActorPos, Vec3 _vFocusPos);
    void UpdateLightLookTo(Vec3 _vActorPos, Vec3 _vFocusDir);

protected:
    std::shared_ptr<ULightComponent> m_pLightComponent;
};