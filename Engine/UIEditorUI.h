#pragma once
#include "PrefabLoader.h"

class UIEditorUI
{
public:
	void DrawUI();
	void DrawVec3(const char* label, float* values);

	void SetOnCreateCallback(std::function<void(shared_ptr<class AUIActor>, const char*, const char*, TransformData)> callback)
	{
		m_OnCreate = std::move(callback);
	}

private:
	void UpdateUIActor();
	void ResetData();
	void SetTexture();
	void SavePrefab();
	void LoadPrefab();

	void SearchFile(const string& directory, const string& extension);

private:
	shared_ptr<class AUIActor> m_pUIActor = nullptr;

	TransformData m_Trans = { { 0.0f, 0.0f, 0.0f },
								{ 0.0f, 0.0f, 0.0f },
								{ 100.0f, 100.0f, 1.0f } };

	int m_iIdleIndex = 0;
	int m_iHoverIndex = 0;
	int m_iActiveIndex = 0;
	int m_iSelectedIndex = 0;

	vector<string> m_vTextureList;
	vector<string> m_vTextureNameList;
	char m_szShaderPath[256] = "../Resources/Shader/Default.hlsl";

	// Prefab
	char m_szPrefabName[64] = "MyUI";

	std::function<void(shared_ptr<AUIActor>, const char*, const char*, TransformData)> m_OnCreate;
};
