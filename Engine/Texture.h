#pragma once
#include "DxTk/WICTextureLoader.h"
#include "DxTk/DDSTextureLoader.h"
#include "Singleton.h"

class Texture
{
public:
	ComPtr<ID3D11Resource> m_pTexture;
	ComPtr<ID3D11ShaderResourceView> m_pTexSRV;

	UINT width = 0;
	UINT height = 0;

public:
	void SetTextureSize();
	void SetSRV(ComPtr<ID3D11ShaderResourceView> _pSRV) { m_pTexSRV = _pSRV; }
	bool LoadTexture(std::wstring _filename);
	Texture() = default;
};

class TextureManager : public Singleton<TextureManager>
{
	map<wstring, shared_ptr<Texture>> m_mList;

public:
	shared_ptr<Texture> Get(wstring _filename);
	shared_ptr<Texture> Load(wstring _filename);
public:
	~TextureManager();
};