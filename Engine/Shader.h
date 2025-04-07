#pragma once
#include "Singleton.h"

class Shader
{
public:
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
public:
	ComPtr<ID3DBlob> m_pCode;
public:
	bool Load(wstring);
	bool CreateVertexShader(wstring);
	bool CreatePixelShader(wstring);
	virtual void Release();
};

class ShaderManager : public Singleton<ShaderManager>
{
	map<wstring, shared_ptr<Shader>> m_mList;
public:
	shared_ptr<Shader> Get(wstring _filename);
	shared_ptr<Shader> Load(wstring _filename);
public:
	~ShaderManager();
};
