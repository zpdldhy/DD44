#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Inputlayout.h"

struct CB_GLOW
{
	float g_fGlowPower;
	DirectX::XMFLOAT3 g_vGlowColor;
};

class UMaterial
{
	shared_ptr<Shader> m_pShader = nullptr;
	shared_ptr<Texture> m_pTexture = nullptr;
	shared_ptr<Inputlayout> m_pInputlayout = nullptr;
	ComPtr<ID3D11Buffer> m_pGlowCB;
public:
	void SetGlowParams(float glowPower, const DirectX::XMFLOAT3& glowColor);
	ComPtr<ID3D11Buffer> GetGlowCB() const { return m_pGlowCB; }
public:
	virtual void Load(wstring _textureFileName, wstring _shaderFileName);
	virtual void SetShader(shared_ptr<Shader> _shader) { m_pShader = _shader; }
	virtual void SetTexture(shared_ptr<Texture> _texture) { m_pTexture = _texture; }

public:
	shared_ptr<Shader> GetShader() { return m_pShader; }
	shared_ptr<Texture> GetTexture() { return m_pTexture; }
	shared_ptr<Inputlayout> GetInputlayout() { return m_pInputlayout; }

public:
	UMaterial() {}
	virtual ~UMaterial() {}
};

