#include "pch.h"
#include "Inputlayout.h"
#include "Device.h"

bool Inputlayout::Load(ComPtr<ID3DBlob> _pCode, D3D11_INPUT_ELEMENT_DESC _layout[], UINT _szNumCounter)
{
	if (_pCode == nullptr)
	{
		return true;
	}
	HRESULT hr = DEVICE->CreateInputLayout(
		_layout,
		_szNumCounter,
		_pCode->GetBufferPointer(),
		_pCode->GetBufferSize(),
		m_pInputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}
	return true;
}

bool InputlayoutManager::CreateInputLayout(wstring layoutName, ComPtr<ID3DBlob> _pCode, D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements)
{
	// 이미 존재하는 경우 반환
	if (Get(layoutName) != nullptr)
	{
		return true;
	}

	shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
	inputlayout->Load(_pCode, layout, numElements);
	m_mList.insert(make_pair(layoutName, inputlayout));

	return true;
}

bool InputlayoutManager::CreateDefault()
{
	if (Get(L"Default") != nullptr)
	{
		return true;
	}
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT iNumCnt = sizeof(layout) / sizeof(layout[0]);
	shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
	inputlayout->Load(m_pCode, layout, iNumCnt);
	m_mList.insert(make_pair(L"Default", inputlayout));

	return true;
}

bool InputlayoutManager::CreateInstance(ComPtr<ID3DBlob> _pCode)
{
	if (Get(L"Instance") != nullptr)
	{
		return true;
	}
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TRANSFORM",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 16,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 32,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 48,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 64,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_UV",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 80,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT iNumCnt = sizeof(layout) / sizeof(layout[0]);
	shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
	inputlayout->Load(_pCode, layout, iNumCnt);
	m_mList.insert(make_pair(L"Instance", inputlayout));

	return true;
}

bool InputlayoutManager::CreateIW(ComPtr<ID3DBlob> _pCode)
{
	auto iter = m_mList.find(L"IW");
	if (iter != m_mList.end())
	{
		return true;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "INDEX",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SECONDI",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,1, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SECONDW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT iNumCnt = sizeof(layout) / sizeof(layout[0]);

	shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
	inputlayout->Load(_pCode, layout, iNumCnt);
	m_mList.insert(make_pair(L"IW", inputlayout));

	return true;
}

bool InputlayoutManager::CreateInstanceIW(ComPtr<ID3DBlob> _pCode)
{
	auto iter = m_mList.find(L"InstanceIW");
	if (iter != m_mList.end())
	{
		return true;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 40,  D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "INDEX",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SECONDI",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,1, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SECONDW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 48,  D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	2, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT,	2, 16,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT,	2, 32,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT,	2, 48,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "FRAME",	0,				DXGI_FORMAT_R32_UINT,	2, 52,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRACK",	0,				DXGI_FORMAT_R32_UINT,	2, 56,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT iNumCnt = sizeof(layout) / sizeof(layout[0]);

	shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
	inputlayout->Load(_pCode, layout, iNumCnt);
	m_mList.insert(make_pair(L"InstanceIW", inputlayout));

	return true;
}

shared_ptr<Inputlayout> InputlayoutManager::Get(wstring _name)
{
	auto target = m_mList.find(_name);
	if (target != m_mList.end())
	{
		return target->second;
	}
	else
	{
		return nullptr;
	}
}

void InputlayoutManager::Init(ComPtr<ID3DBlob> _pCode)
{
	m_pCode = _pCode;
	CreateDefault();
}

InputlayoutManager::~InputlayoutManager()
{
	m_mList.clear();
}
