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
	void SetTexture();

	vector<string> GetTextureList(const std::string& directory);
	void SearchFile(const string& directory, const string& extension);

private:
	shared_ptr<class AUIActor> m_pUIActor = nullptr;

	ActorData m_UIData = { { 0.0f, 0.0f, 0.0f },
								{ 0.0f, 0.0f, 0.0f },
								{ 100.0f, 100.0f, 1.0f } };

	int m_iSelectedTextureIndex = 0;
	vector<string> m_vTextureList;
	vector<string> m_vTextureNameList;
	char m_szShaderPath[256] = "../Resources/Shader/Default.hlsl";

	std::function<void(shared_ptr<AUIActor>, const char*, const char*, ActorData)> m_OnCreate;
};
