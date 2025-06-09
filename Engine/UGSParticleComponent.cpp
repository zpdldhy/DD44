#include "pch.h"
#include "UGSParticleComponent.h"
#include "Device.h"
#include "Timer.h"
#include "UMaterial.h"
#include "RenderStateManager.h"

void UGSParticleComponent::Init()
{
    CreateVertexBuffer();

    // 테스트용 파티클 생성
    for (int i = 0; i < 500; ++i)
    {
        ParticleVertex p;
        p.pos = Vec3(0,10,0);
        p.color = Vec4(1.f, 0.f, 0.f, 1.f);
        p.size = 100000000000.f;
        m_vParticles.push_back(p);
    }

    m_pMaterial = make_shared<UMaterial>();
    m_pMaterial->Load(L"../Resources/Texture/red.png", L"../Resources/Shader/ParticleGS.hlsl", true);

    auto shader = m_pMaterial->GetShader();
    INPUTLAYOUT->CreateParticle(shader->m_pCode);
    m_pMaterial->SetInputlayout(INPUTLAYOUT->Get(L"Particle"));
}

void UGSParticleComponent::Tick()
{
    float t = TIMER->GetGameTime();

    for (auto& p : m_vParticles)
    {
        float offset = sin(t + p.pos.x * 3.0f) * 0.1f;
        p.pos.y += offset; // 살짝 흔들리기
    }
}


void UGSParticleComponent::Render()
{
    if (m_vParticles.empty()) return;

    UpdateVertexBuffer();

    UINT stride = sizeof(ParticleVertex);
    UINT offset = 0;
    DC->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    if (m_pMaterial && m_pMaterial->GetShader())
    {
        // DC->IASetInputLayout(INPUTLAYOUT->Get(L"Particle")->m_pInputLayout.Get());

        auto shader = m_pMaterial->GetShader();
        DC->VSSetShader(shader->m_pVertexShader.Get(), nullptr, 0);
        DC->GSSetShader(shader->m_pGeometryShader.Get(), nullptr, 0); // ← 이거 핵심
        DC->PSSetShader(shader->m_pPixelShader.Get(), nullptr, 0);

        RenderOption particleOption;
        particleOption.blend = BlendType::AlphaBlend;               // 알파 블렌딩
        particleOption.depth = DepthType::ZTestOff;        // ZTest만 켜기
        particleOption.cull = CullType::None;                       // Cull 안함
        particleOption.sampler = SamplerType::Linear;
        particleOption.bWireframe = false;
        particleOption.bStencil = false;
        STATEMANAGER->Apply(particleOption);

        m_pMaterial->Bind();
    }

    DC->Draw(static_cast<UINT>(m_vParticles.size()), 0);

    STATEMANAGER->Restore();

    DC->GSSetShader(nullptr, nullptr, 0);

}

void UGSParticleComponent::Destroy()
{
    m_vParticles.clear();
    m_pVertexBuffer.Reset();
}

bool UGSParticleComponent::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(ParticleVertex) * 10000; // 최대 개수 여유 있게
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = DEVICE->CreateBuffer(&desc, nullptr, m_pVertexBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}


void UGSParticleComponent::UpdateVertexBuffer()
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = DC->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr)) return;

    memcpy(mapped.pData, m_vParticles.data(), sizeof(ParticleVertex) * m_vParticles.size());
    DC->Unmap(m_pVertexBuffer.Get(), 0);
}