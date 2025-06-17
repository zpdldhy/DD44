#pragma once
#include "Singleton.h"

class Shader
{
public:
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
	ComPtr<ID3DBlob> m_pCode;
	wstring m_szPath = L"";

public:
	bool Load(wstring);
	bool CreateVertexShader(wstring);
	bool CreatePixelShader(wstring);
	ComPtr<ID3D11VertexShader> GetVS() const { return m_pVertexShader; }
	ComPtr<ID3D11PixelShader>  GetPS() const { return m_pPixelShader; }
	ComPtr<ID3DBlob>           GetCode() const { return m_pCode; }
	bool Load(wstring _filePath, string _VSEntry, string _PSEntry);

private:
	bool CreateVertexShader(wstring _filePath, string _VSEntry);
	bool CreatePixelShader(wstring _filePath, string _PSEntry);
	virtual void Release();
};

class ShaderManager : public Singleton<ShaderManager>
{
	map<wstring, shared_ptr<Shader>> m_mList;
public:
	shared_ptr<Shader> Get(wstring _filename, wstring _VSEntry = L"VS", wstring _PSEntry = L"PS");

private:
	shared_ptr<Shader> Load(wstring _filename, wstring _VSEntry, wstring _PSEntry);
public:
	~ShaderManager();
};
