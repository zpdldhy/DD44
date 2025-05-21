#pragma once
#include "PrefabLoader.h"

class ParticleEditorUI
{
public:
	void DrawUI();
	void DrawVec3(const char* label, float* values);
	void DrawVec2(const char* label, float* values);


	void SetOnCreateCallback(std::function<void(shared_ptr<class AParticleActor>, const char*, const char*, ActorData, Vec2, Vec2)> callback)
	{
		m_OnCreate = std::move(callback);
	}

private:
	void UpdateUIActor();
	void ResetData();
	void SetTexture();
	vector<string> GetTextureList(const std::string& directory);
	void SearchFile(const string& directory, const string& extension);

	int m_iDivisions = 4; // 기본 3x3 분할
	int m_iSelectedRow = 0;
	int m_iSelectedCol = 0;
private:
	shared_ptr<class AParticleActor> m_pParticleActor = nullptr;

	ActorData m_Data = {
		{ 0.0f, 0.0f, 50.0f }, // Position
		{ 0.0f, 0.0f, 0.0f }, // Rotation
		{ 20.0f, 20.0f, 20.0f }  // Scale
	};

	Vec2 m_UVStart = { 0.0f, 0.0f };
	Vec2 m_UVEnd = { 1.0f, 1.0f };

	int m_iSelectedTextureIndex = 0;
	vector<string> m_vTextureList;
	vector<string> m_vTextureNameList;
	char m_szShaderPath[256] = "../Resources/Shader/SpriteUV.hlsl";

	bool m_bSuppressUpdateThisFrame = false;
	std::vector<shared_ptr<AParticleActor>> m_vCreatedParticles;
	int m_iSelectedParticleIndex = -1;
	int m_iLastSelectedParticleIndex = -1;

	std::function<void(shared_ptr<AParticleActor>, const char*, const char*, ActorData, Vec2, Vec2)> m_OnCreate;
};

