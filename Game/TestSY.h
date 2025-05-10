#pragma once
#include "IExecute.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"
#include "ActorLoader.h"
#include "PrefabLoader.h"

struct FCharacterBuildData
{
	std::shared_ptr<UMeshComponent> RootComponent;
	Vec3 Position;
	Vec3 Rotation;
	Vec3 Scale;
	int ScriptType;
	std::string TexturePath;
	std::string ShaderPath;
	int AnimIndex;
	float AnimSpeed;
};

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

	bool m_bEditorWireframe = false;

	// Mesh, Animation
	std::shared_ptr<ActorLoader> m_pLoader;
	std::shared_ptr<UMeshComponent> m_pRootComponent = nullptr;
	std::vector<std::shared_ptr<UMeshComponent>> m_vMeshList;

	int m_iSelectedMeshIndex = 0;
	int m_iSelectedAnimIndex = 0;
	int m_iSelectedCompType = 0; // 0 = Skinned, 1 = Static
	int m_iSelectedBoneIndex = 0;

	std::vector<std::string> m_vMeshLabels;
	std::vector<const char*> m_vMeshLabelPtrs;
	std::vector<std::string> m_vBoneLabels;
	std::vector<const char*> m_vBoneLabelPtrs;

	//std::shared_ptr<APawn> m_pPreviewActor;
	std::vector<std::shared_ptr<AActor>> m_vPreviewActors;

	// Prefab
	std::vector<PrefabData> m_vPrefabList;
	

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

	int GetBoneIndexByName(const std::wstring& boneName);
};

