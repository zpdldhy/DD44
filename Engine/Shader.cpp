#include "pch.h"
#include "Shader.h"
#include "Device.h"

bool Shader::Load(wstring _filePath, string _VSEntry, string _PSEntry)
{
	m_szPath = _filePath;
	if (!CreateVertexShader(_filePath, _VSEntry))
	{
		return false;
	}
	if (!CreatePixelShader(_filePath, _PSEntry))
	{
		return false;
	}
	return true;
}
bool Shader::CreateVertexShader(std::wstring _filePath, string _VSEntry)
{
	ID3DBlob* errorCode = nullptr;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// 버텍스 쉐이더 파일 컴파일
	HRESULT hr = D3DCompileFromFile(_filePath.c_str(),
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_VSEntry.c_str(), "vs_5_0",
		flags, 0,
		m_pCode.GetAddressOf(), &errorCode);
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}

	// 버텍스 쉐이더 생성
	void* pShaderBytecode = m_pCode->GetBufferPointer();
	SIZE_T BytecodeLength = m_pCode->GetBufferSize();
	ID3D11ClassLinkage* pClassLinkage = nullptr;
	hr = DEVICE->CreateVertexShader(
		pShaderBytecode, BytecodeLength, pClassLinkage,
		m_pVertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}

	if (errorCode) errorCode->Release();
	return true;
}
bool Shader::CreatePixelShader(std::wstring _filePath, string _PSEntry)
{
	// 픽셀 쉐이더 파일 컴파일
	ID3DBlob* code = nullptr;
	ID3DBlob* errorCode = nullptr;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompileFromFile(_filePath.c_str(),
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_PSEntry.c_str(), "ps_5_0",
		flags, 0,
		&code, &errorCode);

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}
	// 픽셀 쉐이더 생성
	void* pShaderBytecode = code->GetBufferPointer();
	SIZE_T BytecodeLength = code->GetBufferSize();
	ID3D11ClassLinkage* pClassLinkage = nullptr;
	hr = DEVICE->CreatePixelShader(
		pShaderBytecode, BytecodeLength, pClassLinkage,
		m_pPixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}

	if (errorCode) errorCode->Release();
	return true;
}
void Shader::Release()
{
}

shared_ptr<Shader> ShaderManager::Get(wstring _filename, wstring _VSEntry, wstring _PSEntry)
{
	wstring name = SplitPath(_filename);
	name += (_VSEntry + _PSEntry);
	auto target = m_mList.find(name);
	if (target != m_mList.end())
	{
		return target->second;
	}
	else
	{
		return Load(_filename, _VSEntry, _PSEntry);
	}
}

shared_ptr<Shader> ShaderManager::Load(wstring _filename, wstring _VSEntry, wstring _PSEntry)
{
	wstring name = SplitPath(_filename);
	name += (_VSEntry + _PSEntry);
	shared_ptr<Shader> shader = make_shared<Shader>();
	if (shader->Load(_filename, to_wm(_VSEntry), to_wm(_PSEntry)))
	{
		m_mList.insert(make_pair(name, shader));
		return shader;
	}
	assert(false);
	return nullptr;
}

ShaderManager::~ShaderManager()
{
	m_mList.clear();
}
