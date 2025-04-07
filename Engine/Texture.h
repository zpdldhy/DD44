#pragma once
#include "DxTk/WICTextureLoader.h"
#include "DxTk/DDSTextureLoader.h"
#include "Singleton.h"

class Texture
{
public:
	ComPtr<ID3D11Resource> _texture;
	ComPtr<ID3D11ShaderResourceView> _texSRV;

	UINT width = 0;
	UINT height = 0;

public:
	void SetTextureSize();
	bool LoadTexture(std::wstring fileName);
	Texture() = default;
};

class TextureManager : public Singleton<TextureManager>
{
	map<wstring, shared_ptr<Texture>> m_mList;

public:
	shared_ptr<Texture> Get(wstring _filename);
	shared_ptr<Texture> Load(wstring filename);
public:
	~TextureManager();
};