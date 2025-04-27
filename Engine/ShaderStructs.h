#pragma once
class ShaderStructs
{
};

struct CB_SLASH
{
    float g_fCutoff;                    // 4
    float g_fGlow;                      // 4
    DirectX::XMFLOAT2 _padding;         // 8 → 16바이트 맞추기

    DirectX::XMFLOAT4 g_vColor;         // 16
    DirectX::XMFLOAT4 g_vOccludedColor; // 16
};

