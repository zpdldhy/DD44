#pragma once
#include "Singleton.h"

class Inputlayout
{
public:
	ComPtr<ID3D11InputLayout> m_pInputLayout;
public:
	virtual bool Load(ComPtr<ID3DBlob> _pCode, D3D11_INPUT_ELEMENT_DESC _layout[], UINT _szNumCounter);
};

class InputlayoutManager : public Singleton<InputlayoutManager>
{
private:
	map<wstring, shared_ptr<Inputlayout>> m_mList;
	ComPtr<ID3DBlob> m_pCode;
public:
	bool CreateInputLayout(wstring layoutName, ComPtr<ID3DBlob> _pCode, D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements);
	bool CreateDefault();
	bool CreateInstance(ComPtr<ID3DBlob> _pCode, wstring _name);
	bool CreateIW(ComPtr<ID3DBlob> _pCode);
	bool CreateInstanceIW(ComPtr<ID3DBlob> _pCode);
	shared_ptr<Inputlayout> Get(wstring _name = L"Default");
	void Init(ComPtr<ID3DBlob> _pCode);
	virtual ~InputlayoutManager();
};

