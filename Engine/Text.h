#pragma once
#include "Singleton.h"

class Font
{
	ComPtr<IDWriteTextFormat>		m_pTextFormat = nullptr;
	ComPtr<ID2D1SolidColorBrush>	m_pColorBrush = nullptr;

	wstring m_szFontPath = L"";
	float m_fFontSize = 20.0f;
	Color m_color = { 0.f, 0.f, 0.f, 1.f };
	DWRITE_TEXT_ALIGNMENT m_horizontal = DWRITE_TEXT_ALIGNMENT_CENTER;
	DWRITE_PARAGRAPH_ALIGNMENT m_vertical = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

public:
	HRESULT Create(const wstring& _szFontPath, float _fFontSize, Color _color);

private:
	HRESULT CreateTextFormat(const wstring& _szFontName, float _fFontSize);

public:
	IDWriteTextFormat* GetTextFormat() const { return m_pTextFormat.Get(); }
	ID2D1SolidColorBrush* GetBrush() const { return m_pColorBrush.Get(); }

	void SetPath(wstring _path);
	void SetSize(float _size);
	void IncreaseSize(float _step);
	void DecreaseSize(float _step);
	void SetColor(Color _color);
	void SetAlignment(DWRITE_TEXT_ALIGNMENT _textAlign, DWRITE_PARAGRAPH_ALIGNMENT _paraAlign);

	wstring GetFontPath() const { return m_szFontPath; }
	float GetFontSize() const { return m_fFontSize; }
	Color GetColor() const { return m_color; }

public:
	Font() = default;
	virtual ~Font();
};

class Text
{
	ComPtr<IDWriteTextLayout>	m_pTextLayout = nullptr;

	Font*	m_pFont = nullptr;
	wstring m_szMessage = L"";
	Vec2	m_vSize = { 0.f, 0.f };

public:
	HRESULT Create(const wstring& _msg, const Vec2& _size, Font* _font);
	void Draw(const Vec2& _position);

public:
	IDWriteTextLayout* GetTextLayout() const { return m_pTextLayout.Get(); }
	void SetFont(Font* font);
	void SetMessage(const wstring& msg);
	void SetSize(const Vec2& size);

public:
	Text() = default;
	virtual ~Text();
};

//class Typer : public Singleton<Typer>
//{
//private:
//	std::wstring fullText;
//	std::wstring currentText;
//	float m_fCharDelay = 0.05f;
//	float m_fTimer = 0.0f;
//	size_t visibleLength = 0;
//	bool m_bFinished = false;
//
//public:
//	void Reset(const std::wstring& newText);
//	void Update(float deltaTime);
//	void Draw(D2D1_RECT_F rect);
//};