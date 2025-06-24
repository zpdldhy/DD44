#include "pch.h"
#include "UGSParticleComponent.h"
#include "Device.h"
#include "Shader.h"
#include "Texture.h"

UGSParticleComponent::UGSParticleComponent()
{
}

void UGSParticleComponent::Init()
{
    m_pShader = SHADER->Get(L"../Resources/Shader/PointParticle.hlsl",L"VS",L"PS",L"GS");

    m_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_bufferDesc.MiscFlags = 0;
    m_bufferDesc.StructureByteStride = sizeof(TPointParticle);
}

void UGSParticleComponent::Tick()
{

}

void UGSParticleComponent::Render()
{
    if (m_particleCount == 0 || !m_pShader)
        return;

    m_pShader->SetVS();
    m_pShader->SetGS();
    m_pShader->SetPS();

    UINT stride = sizeof(TPointParticle);
    UINT offset = 0;
    DC->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    if (m_pTexture)
        DC->PSSetShaderResources(0, 1, m_pTexture->GetSRV().GetAddressOf());

    DC->Draw(m_particleCount, 0);
}

void UGSParticleComponent::SetTexture(shared_ptr<Texture> texture)
{
    m_pTexture = texture;
}

void UGSParticleComponent::SetParticles(const vector<TPointParticle>& particles)
{
    m_vParticleData = particles;
    m_particleCount = static_cast<UINT>(particles.size());

    // 버퍼가 없거나 사이즈가 다르면 재생성
    if (!m_pVertexBuffer || m_bufferDesc.ByteWidth != sizeof(TPointParticle) * m_particleCount)
    {
        m_bufferDesc.ByteWidth = sizeof(TPointParticle) * m_particleCount;
        DEVICE->CreateBuffer(&m_bufferDesc, nullptr, m_pVertexBuffer.GetAddressOf());
    }

    // 데이터 복사
    D3D11_MAPPED_SUBRESOURCE mapped;
    DC->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, m_vParticleData.data(), sizeof(TPointParticle) * m_particleCount);
    DC->Unmap(m_pVertexBuffer.Get(), 0);
}