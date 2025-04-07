#include "pch.h"
#include "Texture.h"
#include "Device.h"

void Texture::SetTextureSize()
{
	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = _texture->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		pTexture->GetDesc(&desc);

		width = desc.Width;
		height = desc.Height;
	}
	pTexture->Release();
}

bool Texture::LoadTexture(std::wstring fileName)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		DEVICE.Get(),
		fileName.c_str(),
		_texture.GetAddressOf(), _texSRV.GetAddressOf());
	if (FAILED(hr))
	{
		hr = DirectX::CreateDDSTextureFromFile(DEVICE.Get(), fileName.c_str(), _texture.GetAddressOf(), _texSRV.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
			return false;
		}
	}
	SetTextureSize();
	return true;
}

shared_ptr<Texture> TextureManager::Get(wstring _filename)
{
	wstring name = SplitPath(_filename);
	auto target = m_mList.find(name);
	if (target != m_mList.end())
	{
		return target->second;
	}
	else
	{
		return Load(_filename);
	}
}

shared_ptr<Texture> TextureManager::Load(wstring _filename)
{
	wstring name = SplitPath(_filename);
	shared_ptr<Texture> texture = make_shared<Texture>();
	if (texture->LoadTexture(_filename))
	{
		m_mList.insert(make_pair(name, texture));
		return texture;
	}
	assert(false);
	return nullptr;
}

TextureManager::~TextureManager()
{
	m_mList.clear();
}
