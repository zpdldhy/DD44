#pragma once
#include "USceneComponent.h"

struct TPointParticle
{
    Vec3 pos;
    Color color;
    float size;
};

class UGSParticleComponent : public USceneComponent
{
public:
    UGSParticleComponent();
    virtual ~UGSParticleComponent() = default;

    virtual void Init() override;
    virtual void Tick() override;
    virtual void Render() override;

public:
    void SetTexture(shared_ptr<class Texture> texture);
    void SetParticles(const vector<TPointParticle>& particles); // 위치 + 컬러 + 크기

private:
    ComPtr<ID3D11Buffer> m_pVertexBuffer;
    D3D11_BUFFER_DESC m_bufferDesc{};
    UINT m_particleCount = 0;

    shared_ptr<class Shader> m_pShader;
    shared_ptr<class Texture> m_pTexture;

    vector<TPointParticle> m_vParticleData;
};

