#pragma once
#include "PrefabLoader.h"

class UIEditorUI
{
public:
	void DrawUI();
	void DrawVec3(const char* label, float* values);

	void SetOnCreateCallback(std::function<void(shared_ptr<class AUIActor>, const char*, const char*, TransformData, Vec4)> callback)
	{
		m_OnCreate = std::move(callback);
	}

private:
	void UpdateUIActor();
	void UpdateUIActorList();
	void UpdatePrefabData();
	void ResetData();
	void SetTransform();
	void SetColor();
	void SetSliceUV();
	void SetTexture();
	void SavePrefab();
	void LoadPrefab();
	void DeletePrefab();

	void SearchFile(const string& _directory, const string& _extension, vector<string>& _vPaths, vector<string>& _vNames);

private:
	void SelectActor();
	void ResolvePrefabData(const PrefabUIData& data);

private:
	vector<shared_ptr<class AUIActor>> m_vUIList;
	shared_ptr<class AUIActor> m_pUIActor = nullptr;
	UINT m_iSelectUIActor = 0;

	PrefabUIData m_CurrentPrefab;

	TransformData m_Trans = { { 0.0f, 0.0f, 0.0f },
								{ 0.0f, 0.0f, 0.0f },
								{ 100.0f, 100.0f, 1.0f } };

	float m_vColor[4] = { 0.f, 0.f, 0.f, 0.f };
	float m_vSliceUV[4] = {0.5f, 0.5f, 0.5f, 0.5f};

	int m_iIdleIndex = 0;
	int m_iHoverIndex = 0;
	int m_iActiveIndex = 0;
	int m_iSelectedIndex = 0;

	vector<string> m_vTextureList;
	vector<string> m_vTextureNameList;

	char m_szShaderPath[256] = "../Resources/Shader/UVSlice.hlsl";

	// Prefab
	char m_szPrefabName[64] = "MyUI";
	vector<string> m_vPrefabList;
	vector<string> m_vPrefabNameList;
	int m_iPrefabdIndex = 0;

	std::function<void(shared_ptr<AUIActor>, const char*, const char*, TransformData, Vec4)> m_OnCreate;

private:
	POINT m_ptCurrentMousePos = { 0, 0 };
	POINT m_ptPrevMousePos = { 0, 0 };
};
