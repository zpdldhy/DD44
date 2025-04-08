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
		_inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		return false;
	}
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
