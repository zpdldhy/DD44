#include "pch.h"
#include "UMaterial.h"
#include "Texture.h"
#include "Shader.h"
#include "Inputlayout.h"

void UMaterial::Load(wstring _textureFileName, wstring _shaderFileName)
{
	m_pShader = SHADER->Load(_shaderFileName);
	m_pTexture = TEXTURE->Load(_textureFileName);
	m_pInputlayout = INPUTLAYOUT->Get();
}
