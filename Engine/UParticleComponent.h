//#pragma once
//#include "USceneComponent.h"
//
//enum class EPARTICLE_TYPE
//{
//    BLOOD,
//    WIND,
//    SPARK,
//    CUSTOM
//};
//
//struct Particle
//{
//    Vec3 position;
//    Vec3 velocity;
//    float lifetime = 1.0f;
//    float age = 0.0f;
//    float size = 1.0f;
//    Vec4 color = Vec4(1, 0, 0, 1); // 붉은 피 색상
//    bool isAlive = true;
//
//    shared_ptr<Texture> texture;
//};
//
//class Texture;
//
//class UParticleComponent :    public USceneComponent
//{
//public:
//    UParticleComponent();
//    virtual ~UParticleComponent() = default;
//
//    virtual void Init() override;
//    virtual void Tick() override;
//    virtual void Render() override;
//
//    void CreateBloodParticles(const Vec3& _origin, int _count);
//
//private:
//    vector<Particle> m_vParticles;
//
//    // GPU 리소스
//    ComPtr<ID3D11Buffer> m_pVertexBuffer;
//    D3D11_BUFFER_DESC m_BufferDesc = {};
//
//    void CreateVertexBuffer();
//    void UpdateVertexBuffer();
//
//    shared_ptr<Texture> m_pParticleTexture;
//};
//
