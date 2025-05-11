#pragma once
#include "Singleton.h"

class Shader;
class UStaticMeshComponent;

struct CB_Blur
{
    Vec2 g_vTexelSize;
    Vec2 g_vDirection;
};

class BlurManager
{
public:
    void Init(UINT width, UINT height);
    void Blur(const ComPtr<ID3D11ShaderResourceView>& input);

    ComPtr<ID3D11ShaderResourceView> GetResultSRV() { return m_pResultSRV; }

private:
    void BlurPass(const ComPtr<ID3D11ShaderResourceView>& input,
        const ComPtr<ID3D11RenderTargetView>& output,
        Vec2 direction);

private:
    // Render targets and intermediate
    ComPtr<ID3D11Texture2D> m_pTempTexture;
    ComPtr<ID3D11RenderTargetView> m_pTempRTV;
    ComPtr<ID3D11ShaderResourceView> m_pTempSRV;

    ComPtr<ID3D11Texture2D> m_pResultTexture;
    ComPtr<ID3D11RenderTargetView> m_pResultRTV;
    ComPtr<ID3D11ShaderResourceView> m_pResultSRV;

    // ConstantBuffer
    ComPtr<ID3D11Buffer> m_pBlurCB;

    // Shader and fullscreen quad
    shared_ptr<Shader> m_pBlurShader;
    shared_ptr<UStaticMeshComponent> m_pScreenQuad;
};