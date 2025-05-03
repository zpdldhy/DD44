#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Inputlayout.h"

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

class UMaterial
{
	shared_ptr<Shader> m_pShader = nullptr;
	shared_ptr<Texture> m_pTexture = nullptr;
	shared_ptr<Inputlayout> m_pInputlayout = nullptr;
	ComPtr<ID3D11Buffer> m_pGlowCB;
	ComPtr<ID3D11Buffer> m_pDissolveCB;
	ComPtr<ID3D11ShaderResourceView> m_pNoiseSRV;
	ComPtr<ID3D11Buffer> m_pUVDistortionCB;
	ComPtr<ID3D11Buffer> m_pCameraCB;
public:
	void SetGlowParams(float _glowPower, const Vec3 _glowColor);
	void SetHitFlashTime(float _flashTime);
	void UpdateGlowBuffer();
	void UpdateDissolveBuffer();
	void UpdateUVDistortionBuffer(float _deltaTime);
	void UpdateCameraBuffer();
	void CreateGlowCB();
	void CreateDissolveCB();
	void CreateUVDistortionCB();
	void CreateCameraCB();
	void SetDissolveParams(float _threshold);
	void SetNoiseTexture(std::shared_ptr<Texture> _tex);
	void SetUVDistortionParams(float _strength, float _speed, float _frequency);
	void SetCameraPos(const Vec3& _cameraPos);
	

public:
	CB_GLOW m_tGlowData = { 0.0f, };
	CB_DISSOLVE m_tDissolveData = { 0.0f };
	CB_UVDistortion m_tUVDistortion = { };
	CB_CAMERA m_tCameraData;

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

public:
	UMaterial() {}
	virtual ~UMaterial() {}
};

