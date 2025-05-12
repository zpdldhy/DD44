#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Inputlayout.h"

enum class ERenderMode
{
	Default = 0,
	Silhouette = 1,
};

struct CB_GLOW
{
	float g_fGlowPower;
	float padding[3];
	Vec3 g_vGlowColor;
	float dummy = 0.0f;
	float g_fHitFlashTime;
	float padding2[3];
};

struct CB_DISSOLVE
{
	float g_fDissolveThreshold;
	float padding[3];
};

struct CB_UVDistortion
{
	float g_fDistortionStrength;
	float g_fWaveSpeed;
	float g_fWaveFrequency;
	float g_fDistortionTime;
};

struct CB_CAMERA
{
	Vec3 g_vCameraPos;
	float padding;
};

struct CB_RMB
{
	int iRenderMode = 0;
	Vec3 padding;
};

struct CB_EMISSIVE
{
	Vec3 g_vEmissiveColor = Vec3(0, 0, 0);
	float g_fEmissivePower = 0.0f;
};

class UMaterial
{
	bool m_bUseStencil = false;

	std::wstring m_TexturePath;
	std::wstring m_ShaderPath;

	shared_ptr<Shader> m_pShader = nullptr;
	shared_ptr<Texture> m_pTexture = nullptr;
	shared_ptr<Inputlayout> m_pInputlayout = nullptr;
	ComPtr<ID3D11Buffer> m_pGlowCB;
	ComPtr<ID3D11Buffer> m_pDissolveCB;
	ComPtr<ID3D11ShaderResourceView> m_pNoiseSRV;
	ComPtr<ID3D11Buffer> m_pUVDistortionCB;
	ComPtr<ID3D11Buffer> m_pCameraCB;
	ComPtr<ID3D11Buffer> m_pRenderModeBuffer;
	ComPtr<ID3D11Buffer> m_pEmissiveCB;
public:
	void SetGlowParams(float _glowPower, const Vec3 _glowColor);
	void SetHitFlashTime(float _flashTime);
	void UpdateGlowBuffer();
	void UpdateDissolveBuffer();
	void UpdateUVDistortionBuffer(float _deltaTime);
	void UpdateCameraBuffer();
	void UpdateRenderModeBuffer();
	void UpdateEmissiveBuffer();
	void CreateGlowCB();
	void CreateDissolveCB();
	void CreateUVDistortionCB();
	void CreateCameraCB();
	void CreateRenderModeCB();
	void CreateEmissiveCB();
	void SetDissolveParams(float _threshold);
	void SetNoiseTexture(std::shared_ptr<Texture> _tex);
	void SetUVDistortionParams(float _strength, float _speed, float _frequency);
	void SetCameraPos(const Vec3& _cameraPos);
	void SetRenderMode(ERenderMode _eMode);
	void SetEmissiveParams(const Vec3& _color, float _power);

	void SetUseStencil(bool _bUseStencil) { m_bUseStencil = _bUseStencil; }
	bool IsUseStencil() { return m_bUseStencil; }
	

public:
	CB_GLOW m_tGlowData = { 0.0f, };
	CB_DISSOLVE m_tDissolveData = { 0.0f };
	CB_UVDistortion m_tUVDistortion = { };
	CB_CAMERA m_tCameraData;
	CB_RMB m_tRenderModeData;
	ERenderMode m_eRenderMode = ERenderMode::Default;
	CB_EMISSIVE m_tEmissiveData;

public:
	virtual void Load(wstring _textureFileName, wstring _shaderFileName);
	virtual void Bind();

public:
	virtual void SetShader(shared_ptr<Shader> _shader) { m_pShader = _shader; }
	virtual void SetTexture(shared_ptr<Texture> _texture) { m_pTexture = _texture; }
	virtual void SetInputlayout(shared_ptr<Inputlayout> _inputlayout) { m_pInputlayout = _inputlayout; }

public:
	shared_ptr<Shader> GetShader() { return m_pShader; }
	shared_ptr<Texture> GetTexture() { return m_pTexture; }
	shared_ptr<Inputlayout> GetInputlayout() { return m_pInputlayout; }
	const std::wstring& GetTexturePath() const { return m_TexturePath; }
	const std::wstring& GetShaderPath() const { return m_ShaderPath; }

public:
	UMaterial() {}
	virtual ~UMaterial() {}
};

