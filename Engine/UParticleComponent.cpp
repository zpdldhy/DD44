#include "pch.h"
//#include "UParticleComponent.h"
//#include "Device.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "Timer.h"
//
//UParticleComponent::UParticleComponent()
//{
//	CreateVertexBuffer();
//}
//
//void UParticleComponent::Init()
//{
//	m_pParticleTexture = make_shared<Texture>();
//	m_pParticleTexture->LoadTexture(L"../Resources/Textures/wind.png");
//}
//
//void UParticleComponent::Tick()
//{
//    float dt = TIMER->GetDeltaTime();
//
//    for (auto& p : m_vParticles)
//    {
//        if (!p.isAlive) continue;
//
//        p.age += dt;
//        if (p.age >= p.lifetime)
//        {
//            p.isAlive = false;
//            continue;
//        }
//
//        // Update position
//        p.position += p.velocity * dt;
//
//        // 중력 적용 (간단히 y축만)
//        p.velocity.y -= 9.8f * dt;
//    }
//}
//
//void UParticleComponent::Render()
//{
//    UpdateVertexBuffer();
//
//    // 텍스처 세팅
//    if (m_pParticleTexture)
//        m_pParticleTexture->Set(0);
//
//    // Shader 세팅, 알파 블렌딩 켜기 등등 (생략: 필요 시 추가)
//
//    DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//    UINT stride = sizeof(Vec3);
//    UINT offset = 0;
//    DC->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
//
//    DC->Draw(static_cast<UINT>(m_vParticles.size()), 0);
//}
//
//void UParticleComponent::CreateBloodParticles(const Vec3& _origin, int _count)
//{
//}
//
//void UParticleComponent::CreateVertexBuffer()
//{
//}
//
//void UParticleComponent::UpdateVertexBuffer()
//{
//}
