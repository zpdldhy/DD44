#pragma once
#include "Singleton.h"

class Inputlayout
{
private:
	ComPtr<ID3D11InputLayout> _inputLayout;
public:
	virtual bool Load(ComPtr<ID3DBlob> _pCode, D3D11_INPUT_ELEMENT_DESC _layout[], UINT _szNumCounter);
	ComPtr<ID3D11InputLayout> GetInputlayout() { return _inputLayout; }
};

class InputlayoutManager : public Singleton<InputlayoutManager>
{
private:
	map<wstring, shared_ptr<Inputlayout>> m_mList;
	ComPtr<ID3DBlob> m_pCode;
public:
	bool CreateDefault();
	ComPtr<ID3D11InputLayout> Get(wstring _name = L"Default");
	void Init(ComPtr<ID3DBlob> _pCode);
	virtual ~InputlayoutManager();
};

