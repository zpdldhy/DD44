#pragma once
#include "AActor.h"
#include "ULightComponent.h"

class ALight : public AActor
{
public:
    ALight();
    virtual ~ALight() = default;

    std::shared_ptr<ULightComponent> GetLightComponent() const { return m_pLightComponent; }

protected:
    std::shared_ptr<ULightComponent> m_pLightComponent;
};