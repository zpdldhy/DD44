#pragma once
#include "Singleton.h"

class Shader
{
public:
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;
public:
	ComPtr<ID3DBlob> m_pCode;
public:
	bool Load(wstring, bool bUseGeometryShader = false);
	bool CreateVertexShader(wstring);
	bool CreatePixelShader(wstring);
	bool CreateGeometryShader(wstring);
	virtual void Release();

	
};

class ShaderManager : public Singleton<ShaderManager>
{
	map<wstring, shared_ptr<Shader>> m_mList;
public:
	shared_ptr<Shader> Get(wstring _filename, bool bUseGeometryShader = false);
	shared_ptr<Shader> Load(wstring _filename, bool bUseGeometryShader = false);
public:
	~ShaderManager();
};
