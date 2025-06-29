#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Inputlayout.h"
#include "ConstantBuffer.h"

struct CB_MaterialEffect
{
	float g_fGlowPower;
	float padding0[3];
	Vec3 g_vGlowColor;
	float g_fHitFlashTime;

	float g_fDistortionStrength;
	float g_fWaveSpeed;
	float g_fWaveFrequency;
	float g_fDistortionTime;

	Vec3   g_vEmissiveColor;
	float  g_fEmissivePower;

	Vec3   g_vAmbientCoeff = Vec3(1,1,1);    
	float  g_fAmbientPower = 1.0f;

	Vec3   g_vDiffuseCoeff = Vec3(1, 1, 1);
	float  g_fDiffusePower = 1.0f;

	Vec3   g_vSpecularCoeff = Vec3(0.2f,0.2f,0.2f);   
	float  g_fShininess = 32.0f;

	Vec4 TintColor = Vec4(1, 1, 1, 1);

	BOOL g_bCrash = false;
	Vec3 padding_Crash;

	float g_fDissolveAmount = 0.0f;
	float g_fDissolveEdgeWidth = 0.05f;
	Vec3  g_vDissolveColor = Vec3(1, 0.4f, 0.0f);
	Vec3  padding_Dissolve;

};

//struct CB_Material
//{
//	Vec4 vAmbient;
//	Vec4 vDiffuse;
//	Vec4 vSpecular;
//	Vec4 vEmissive;
//};

// 현재 dissolve기능 삭제
//struct CB_DISSOLVE
//{
//	float g_fDissolveThreshold;
//	float padding[3];
//};

struct CB_Slash
{
	float g_fProgress = 0.0f;
	BOOL reverse = false;
	Vec2 paading;
};

class UMaterial
{
	bool m_bUseEffect = true;

	std::wstring m_TexturePath;
	std::wstring m_ShaderPath;

	shared_ptr<Shader> m_pShader = nullptr;
	shared_ptr<Texture> m_pTexture = nullptr;
	shared_ptr<Inputlayout> m_pInputlayout = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_pTexSRV;
	
	ComPtr<ID3D11Buffer> m_pEffectCB; // 통합 CB
	ComPtr<ID3D11Buffer> m_pCB_Slash;

	CB_MaterialEffect m_tEffectData = {};

	CB_Slash m_tSlashData;

public:
	virtual void Load(wstring _textureFileName, wstring _shaderFileName);
	virtual void Bind();

	void CreateEffectCB();
	void CreateSlashCB();

	void UpdateEffectBuffer();
	void SetSlashProgress(float _progress, bool _reverse);

	void SetEmissiveParams(const Vec3& _color, float _power);
	void SetGlowParams(float _glowPower, const Vec3 _glowColor);
	void SetHitFlashTime(float _flashTime);
	void SetUVDistortionParams(float _strength, float _speed, float _frequency);
	void SetAmbientParams(const Vec3& _coeff, float _power);
	void SetDiffuseParams(const Vec3& _coeff, float _power);
	void SetSpecularParams(const Vec3& _coeff, float _shininess = 10.f);
	void SetUseEffect(bool _bUseEffect) { m_bUseEffect = _bUseEffect; }
	void SetIsMetal(bool _bMetal, float _shininess);
	bool IsUseEffect() { return m_bUseEffect; }
	void SetTintColor(const Vec4& color);
	void SetCrash(bool _bCrash);
	void SetDissolve(float _amount);

	// 캐릭터 인스턴싱을 위해
	float GetFlashTIme() { return m_tEffectData.g_fHitFlashTime; }
	bool GetCrack() { return  m_tEffectData.g_bCrash; }
	float GetDissolve() { return m_tEffectData.g_fDissolveAmount; }
public:
	virtual void SetShader(shared_ptr<Shader> _shader) { m_pShader = _shader; }
	//void SetShader(const std::wstring& path);
	virtual void SetTexture(shared_ptr<Texture> _texture) { m_pTexture = _texture; }
	void SetTexture(ID3D11ShaderResourceView* srv) { m_pTexSRV = srv; }
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

