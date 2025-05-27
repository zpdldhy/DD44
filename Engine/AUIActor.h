#pragma once
#include "AActor.h"
#include "UIState.h"
#include "PrefabData.h"
#include "Text.h"

struct UIData
{
	// x는 left, y는 right, z는 top, w는 bottom
	Vec4 vSlice;
	Color vColor;
};

class Texture;

class AUIActor : public AActor
{
public:
	AUIActor() { m_pState = make_shared<IdleUIState>(); m_eStateType = UIStateType::ST_IDLE; }
	virtual ~AUIActor() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;

private:
	void TextRender();
	void CreateUISlice();
	void CreateText();
	void UpdateText();

public:
	void SetState(shared_ptr<UIState> _pState) { m_pState = _pState; }
	void SetStateType(UIStateType _eStateType) { m_eStateType = _eStateType; }
	UIStateType GetStateType() { return m_eStateType; }

	void SetIdleTexture(shared_ptr<Texture> _pTexture) { m_pIdleTexture = _pTexture; }
	void SetHoverTexture(shared_ptr<Texture> _pTexture) { m_pHoverTexture = _pTexture; }
	void SetActiveTexture(shared_ptr<Texture> _pTexture) { m_pActiveTexture = _pTexture; }
	void SetSelectTexture(shared_ptr<Texture> _pTexture) { m_pSelectTexture = _pTexture; }

	// 9-slice
	void SetSliceData(const Vec4& _vSlice) { m_tUISliceData.vSlice = _vSlice; }
	void SetColor(const Color& _vColor) { m_tUISliceData.vColor = _vColor; }
	void AddColor(const Color& _vColor){ m_tUISliceData.vColor += _vColor; }
	Color GetColor() { return m_tUISliceData.vColor; }

	// Text
	void SetText(const wstring& _szText) { m_szText = _szText; }


	// Prefab
	void SetPrefabData(const PrefabUIData& _data) { m_PrefabData = _data; }
	PrefabUIData GetPrefabData() { return m_PrefabData; }

public:
	bool m_bTextureUI = false;

protected:
	// Constants
	UIData m_tUISliceData = { {0.5f, 0.5f, 0.5f, 0.5f}, {0.f, 0.f, 0.f, 0.f} };
	static ComPtr<ID3D11Buffer> m_pUISliceCB;

	// State
	shared_ptr<UIState> m_pState = nullptr;
	UIStateType m_eStateType = UIStateType::ST_IDLE;

	// Texture
	shared_ptr<Texture> m_pIdleTexture = nullptr;
	shared_ptr<Texture> m_pHoverTexture = nullptr;
	shared_ptr<Texture> m_pActiveTexture = nullptr;
	shared_ptr<Texture> m_pSelectTexture = nullptr;

	// Text
	unique_ptr<Font> m_pFont = nullptr;
	unique_ptr<Text> m_pText = nullptr;
	wstring m_szFontPath = L"../Resources/Font/LibreBaskerville-Regular.ttf";
	wstring m_szText = L"Hello World";

	// Prefab
	PrefabUIData m_PrefabData;
};