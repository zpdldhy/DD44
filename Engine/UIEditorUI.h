#pragma once
#include "PrefabLoader.h"

class UIEditorUI
{
public:
	void DrawUI();
	void DrawVec3(const char* label, float* values);

	void SetOnCreateCallback(std::function<void(shared_ptr<class AUIActor>, const char*, const char*, ActorData)> callback)
	{
		m_OnCreate = std::move(callback);
	}

private:
	void UpdateUIActor();
	void ResetData();

private:
	shared_ptr<class AUIActor> m_pUIActor = nullptr;

	ActorData m_UIData = { { 0.0f, 0.0f, 0.0f },
								{ 0.0f, 0.0f, 0.0f },
								{ 100.0f, 100.0f, 1.0f } };

	char m_szTexturePath[256] = "../Resources/Texture/white.png";
	char m_szShaderPath[256] = "../Resources/Shader/Default.hlsl";

	std::function<void(shared_ptr<AUIActor>, const char*, const char*, ActorData)> m_OnCreate;
};
