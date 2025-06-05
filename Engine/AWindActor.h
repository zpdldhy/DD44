#pragma once
#include "AActor.h"

struct WindData
{
    Vec2 vUVStart;
    Vec2 vUVSize;
    Vec2 vUVOffset;
    Vec2 padding;
};

class AWindActor : public AActor
{
private:
    static ComPtr<ID3D11Buffer> m_pWindCB;

    shared_ptr<class UMaterial> m_pMaterial;
    WindData m_tWindData;

    Vec2 m_vSize = Vec2(0.3f, 0.1f); // NDC 단위 크기
    float m_fSpeed = 0.2f;
    float m_fTimeAcc = 0.f;

public:
    AWindActor();

    virtual void Init() override;
    virtual void Tick() override;
    virtual void Render() override;

    void CreateWindCB();

    void SetUV(const Vec2& _start, const Vec2& _size);
    void SetSize(const Vec2& _size);
    void SetSpeed(float _speed);
};