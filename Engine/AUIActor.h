#pragma once
#include "AActor.h"
#include "UIState.h"
#include "PrefabData.h"
#include "Text.h"

class Texture;
class UStaticMeshComponent;

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
	void CreateUVSlice();
	void UpdateUVSlice();
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
	void SetAllTexture(shared_ptr<Texture> _pTexture) { m_pIdleTexture = m_pHoverTexture = m_pActiveTexture = m_pSelectTexture = _pTexture; }

	// 9-slice
	void SetSliceData(const Vec4& _vSlice) { m_vSlice = _vSlice; }
	void SetColor(const Color& _vColor) { m_Color = _vColor; }
	void AddColor(const Color& _vColor){ m_Color += _vColor; }
	Color GetColor() { return m_Color; }

	// Text
	void SetFontSize(const float& _scale) { m_fFontSize = _scale; }
	float GetFontSize() { return m_fFontSize; }
	void SetFontPath(const wstring& _szPath) { m_szFontPath = _szPath; }
	wstring GetFontPath() { return m_szFontPath; }
	void SetText(const wstring& _szText) { m_szText = _szText; }
	wstring GetText() { return m_szText; }

	// Prefab
	void SetPrefabData(const PrefabUIData& _data) { m_PrefabData = _data; }
	PrefabUIData GetPrefabData() { return m_PrefabData; }

public:
	bool m_bTextUI = false;

protected:
	Color m_Color = { 0.f, 0.f, 0.f, 1.f };

	// State
	shared_ptr<UIState> m_pState = nullptr;
	UIStateType m_eStateType = UIStateType::ST_IDLE;

	// Texture
	shared_ptr<Texture> m_pIdleTexture = nullptr;
	shared_ptr<Texture> m_pHoverTexture = nullptr;
	shared_ptr<Texture> m_pActiveTexture = nullptr;
	shared_ptr<Texture> m_pSelectTexture = nullptr;

	// Slice-9
	vector<shared_ptr<UStaticMeshComponent>> m_vSliceMesh;
	shared_ptr<class AInstance> m_pSliceActor = nullptr;

	// 픽셀 단위로 생각 할 것.
	// x는 left, y는 right, z는 top, w는 bottom
	Vec4 m_vSlice = { 0.5f, 0.5f, 0.5f, 0.5f };

	// Font
	wstring m_szFontPath = L"../Resources/Font/LibreBaskerville-Regular.ttf";
	float m_fFontSize = 20.0f;

	// Text
	unique_ptr<Font> m_pFont = nullptr;
	unique_ptr<Text> m_pText = nullptr;
	wstring m_szText = L"Hello World";

	// Prefab
	PrefabUIData m_PrefabData;
};