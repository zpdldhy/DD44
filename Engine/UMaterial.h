#pragma once
class UMaterial
{
	shared_ptr<class Shader> m_pShader = nullptr;
	shared_ptr<class Texture> m_pTexture = nullptr;
	shared_ptr<class Inputlayout> m_pInputlayout = nullptr;

public:
	virtual void Load(wstring _textureFileName, wstring _shaderFileName);
	virtual void SetShader(shared_ptr<class Shader> _shader) { m_pShader = _shader; }
	virtual void SetTexture(shared_ptr<class Texture> _texture) { m_pTexture = _texture; }

public:
	shared_ptr<class Shader> GetShader() { return m_pShader; }
	shared_ptr<class Texture> GetTexture() { return m_pTexture; }
	shared_ptr<class Inputlayout> GetInputlayout() { return m_pInputlayout; }

public:
	UMaterial() {}
	virtual ~UMaterial() {}
};

