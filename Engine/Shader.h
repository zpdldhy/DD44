#pragma once
#include "Singleton.h"

class Shader
{
public:
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
	ComPtr<ID3DBlob> m_pCode;

public:
	bool Load(wstring);
	bool CreateVertexShader(wstring);
	bool CreatePixelShader(wstring);
	virtual void Release();

	ComPtr<ID3DBlob> CompileVS(const std::wstring& file, const std::string& entry);
	ComPtr<ID3DBlob> CompileGS(const std::wstring& file, const std::string& entry, bool streamOut);
	ComPtr<ID3DBlob> CompilePS(const std::wstring& file, const std::string& entry);

	ComPtr<ID3D11GeometryShader> CreateGeometryShader(ComPtr<ID3DBlob> blob);
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
