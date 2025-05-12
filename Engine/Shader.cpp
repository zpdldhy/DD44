#include "pch.h"
#include "Shader.h"
#include "Device.h"

bool Shader::Load(std::wstring _filename)
{
	if (!CreateVertexShader(_filename))
	{
		return false;
	}
	if (!CreatePixelShader(_filename))
	{
		return false;
	}
	return true;
}
bool Shader::CreateVertexShader(std::wstring _filename)
{
	ID3DBlob* errorCode = nullptr;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// ���ؽ� ���̴� ���� ������
	HRESULT hr = D3DCompileFromFile(_filename.c_str(),
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0",
		flags, 0,
		m_pCode.GetAddressOf(), &errorCode);
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}

	// ���ؽ� ���̴� ����
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
bool Shader::CreatePixelShader(std::wstring _filename)
{
	// �ȼ� ���̴� ���� ������
	ID3DBlob* code = nullptr;
	ID3DBlob* errorCode = nullptr;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompileFromFile(_filename.c_str(),
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0",
		flags, 0,
		&code, &errorCode);

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}
	// �ȼ� ���̴� ����
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

ComPtr<ID3D11GeometryShader> Shader::CreateGeometryShader(ComPtr<ID3DBlob> blob)
{
	ComPtr<ID3D11GeometryShader> gs;
	HRESULT hr = DEVICE->CreateGeometryShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		gs.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return nullptr;
	}
	return gs;
}

ComPtr<ID3DBlob> Shader::CompileVS(const std::wstring& file, const std::string& entry)
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> errorBlob;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompileFromFile(file.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry.c_str(), "vs_5_0", flags, 0, blob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return nullptr;
	}
	return blob;
}

ComPtr<ID3DBlob> Shader::CompileGS(const std::wstring& file, const std::string& entry, bool streamOut)
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> errorBlob;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompileFromFile(file.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry.c_str(), "gs_5_0", flags, 0, blob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return nullptr;
	}
	return blob;
}

ComPtr<ID3DBlob> Shader::CompilePS(const std::wstring& file, const std::string& entry)
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> errorBlob;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = D3DCompileFromFile(file.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry.c_str(), "ps_5_0", flags, 0, blob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return nullptr;
	}
	return blob;
}

void Shader::Release()
{
}

shared_ptr<Shader> ShaderManager::Get(wstring _filename)
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

shared_ptr<Shader> ShaderManager::Load(wstring _filename)
{
	wstring name = SplitPath(_filename);
	shared_ptr<Shader> shader = make_shared<Shader>();
	if (shader->Load(_filename))
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
