#include "pch.h"
#include "BlurManager.h"
#include "Device.h"
#include "Shader.h"
#include "UStaticMeshComponent.h"
#include "Inputlayout.h"

void BlurManager::Init(UINT width, UINT height)
{
    // 렌더 타겟 텍스처 설정
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // Temp 텍스처
    DEVICE->CreateTexture2D(&texDesc, nullptr, m_pTempTexture.GetAddressOf());
    DEVICE->CreateRenderTargetView(m_pTempTexture.Get(), nullptr, m_pTempRTV.GetAddressOf());
    DEVICE->CreateShaderResourceView(m_pTempTexture.Get(), nullptr, m_pTempSRV.GetAddressOf());

    // Result 텍스처
    DEVICE->CreateTexture2D(&texDesc, nullptr, m_pResultTexture.GetAddressOf());
    DEVICE->CreateRenderTargetView(m_pResultTexture.Get(), nullptr, m_pResultRTV.GetAddressOf());
    DEVICE->CreateShaderResourceView(m_pResultTexture.Get(), nullptr, m_pResultSRV.GetAddressOf());

    // Blur용 ConstantBuffer 생성 (b3)
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_Blur);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    DEVICE->CreateBuffer(&bd, nullptr, m_pBlurCB.GetAddressOf());

    // 셰이더 로드
    m_pBlurShader = SHADER->Load(L"../Resources/Shader/Blur.hlsl");
    m_pCombineShader = SHADER->Load(L"../Resources/Shader/CombineShader.hlsl");
    // 스크린 쿼드 생성
    m_pScreenQuad = UStaticMeshComponent::CreatePlane(); // 화면용 Quad
}

void BlurManager::Blur(const ComPtr<ID3D11ShaderResourceView>& input)
{
    // 수평 → 수직 2Pass Blur
    BlurPass(input, m_pTempRTV, Vec2(1, 0));
    BlurPass(m_pTempSRV, m_pResultRTV, Vec2(0, 1));
}

void BlurManager::BlurPass(const ComPtr<ID3D11ShaderResourceView>& input,
    const ComPtr<ID3D11RenderTargetView>& output,
    Vec2 direction)
{
    // 1. ConstantBuffer (b3) 업데이트
    D3D11_TEXTURE2D_DESC texDesc = {};
    m_pTempTexture->GetDesc(&texDesc);

    CB_Blur blurData = {};
    blurData.g_vTexelSize = Vec2(1.0f / texDesc.Width, 1.0f / texDesc.Height);
    blurData.g_vDirection = direction;
    DC->UpdateSubresource(m_pBlurCB.Get(), 0, nullptr, &blurData, 0, 0);
    DC->PSSetConstantBuffers(3, 1, m_pBlurCB.GetAddressOf());

    // 2. 렌더 타겟 설정
    DC->OMSetRenderTargets(1, output.GetAddressOf(), nullptr);
    float clearColor[4] = { 0, 0, 0, 0 };
    DC->ClearRenderTargetView(output.Get(), clearColor);

    // 3. 셰이더 바인딩
    if (m_pBlurShader)
    {
        DC->VSSetShader(m_pBlurShader->m_pVertexShader.Get(), nullptr, 0);
        DC->PSSetShader(m_pBlurShader->m_pPixelShader.Get(), nullptr, 0);
    }

    // 4. InputLayout 바인딩
    DC->IASetInputLayout(INPUTLAYOUT->Get(L"Default")->m_pInputLayout.Get());

    // 5. 입력 텍스처 바인딩
    DC->PSSetShaderResources(0, 1, input.GetAddressOf());

    // 6. 스크린 쿼드 렌더
    m_pScreenQuad->Render();

    // 7. SRV 해제
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    DC->PSSetShaderResources(0, 1, nullSRV);
}

void BlurManager::RenderCombine(const ComPtr<ID3D11ShaderResourceView>& sceneSRV)
{
    // 1. 백버퍼에 렌더할 준비
    ID3D11RenderTargetView* backBufferRTV = GET_SINGLE(Device)->GetBackBufferRTV();
    DC->OMSetRenderTargets(1, &backBufferRTV, GET_SINGLE(Device)->GetDepthStencilView());

    // 2. CombineShader 바인딩
    if (m_pCombineShader)
    {
        DC->VSSetShader(m_pCombineShader->m_pVertexShader.Get(), nullptr, 0);
        DC->PSSetShader(m_pCombineShader->m_pPixelShader.Get(), nullptr, 0);

    }

    // 2.5. InputLayout 바인딩 (combine용 vertex layout)
    DC->IASetInputLayout(INPUTLAYOUT->Get(L"Default")->m_pInputLayout.Get());
 
    // 3. Scene + Bloom 결과 바인딩 (t0, t1)
    DC->PSSetShaderResources(0, 1, sceneSRV.GetAddressOf());        // t0
    DC->PSSetShaderResources(1, 1, m_pResultSRV.GetAddressOf());    // t1

    // 4. 전체화면 쿼드 렌더
    m_pScreenQuad->Render();


    // 5. SRV 해제
    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
    DC->PSSetShaderResources(0, 2, nullSRV);
}