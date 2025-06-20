#pragma once
#include "DxTk/WICTextureLoader.h"
#include "DxTk/DDSTextureLoader.h"
#include "Singleton.h"

class Texture
{
public:
	ComPtr<ID3D11Resource> m_pTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_pTexSRV = nullptr;
	wstring m_pFilePath;

	UINT width = 0;
	UINT height = 0;

private:
	void SetTextureSize();	

public:
	bool LoadTexture(std::wstring _filename);
	Texture() = default;

	ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_pTexSRV; }
	ID3D11ShaderResourceView* GetSRVRaw() { return m_pTexSRV.Get(); }
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