#pragma once
#include "AActor.h"

class UMaterial;

class AWindQuadActor : public AActor
{
private:
    shared_ptr<UMaterial> m_pMaterial;

public:
    virtual void Init() override;
    virtual void Render() override;

    void SetSRV(ID3D11ShaderResourceView* _srv);
};