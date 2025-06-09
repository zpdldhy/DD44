#pragma once
#include "UActorComponent.h"

class UMaterial;

struct ParticleVertex
{
    Vec3 pos;
    Vec4 color;
    float size;
};

class UGSParticleComponent : public UActorComponent
{
public:
    void Init() override;
    void Tick() override;
    void Render() override;
    void Destroy() override;

private:
    vector<ParticleVertex> m_vParticles;
    ComPtr<ID3D11Buffer> m_pVertexBuffer;

    bool CreateVertexBuffer();
    void UpdateVertexBuffer();

    shared_ptr<UMaterial> m_pMaterial;
};