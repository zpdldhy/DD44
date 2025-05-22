#include "pch.h"
#include "CharacterEditorUI.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "AnimTrack.h"
#include "Inputlayout.h"
#include "AAsset.h"

bool CharacterEditorUI::m_bRootSet = false;
bool CharacterEditorUI::m_bChildAttached = false;
int CharacterEditorUI::m_iChildIndex = 0;

void CharacterEditorUI::DrawUI()
{
	static std::vector<std::string> meshDisplayNames;
	static std::vector<const char*> meshDisplayNamePtrs;

	// ────────────────────────── Asset Paths ─────────────────────────────
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset Paths");
	ImGui::InputText(" ", m_szAssetPath, IM_ARRAYSIZE(m_szAssetPath));

	if (ImGui::Button("Load Asset"))
	{
		meshDisplayNames.clear();
		meshDisplayNamePtrs.clear();

		if (!m_pLoader)
			m_pLoader = std::make_shared<ActorLoader>();

		// 다른 obj 파일의 mesh도 사용이 될 수 있어서 LoadAll로 해둠
		m_pLoader->LoadAllAsset();

		m_mMeshMap = m_pLoader->LoadMeshMap();
		m_mAnimMap = m_pLoader->LoadAnimMap();
		m_vMeshDataList = m_pLoader->LoadMeshData();

		meshLoader.SetMesh(m_mMeshMap);
		meshLoader.SetAnim(m_mAnimMap);


		string name = to_wm(SplitName(to_mw(m_szAssetPath)));
		string meshPath = "../Resources/Asset/";
		meshPath += name;
		meshPath += ".mesh.json";
		strcpy_s(m_szMeshPath, meshPath.c_str());
	}

	if (!m_vMeshDataList.empty())
	{
		meshDisplayNames.clear();
		meshDisplayNamePtrs.clear();
		for (size_t i = 0; i < m_vMeshDataList.size(); ++i)
		{
			auto mesh = m_vMeshDataList[i];
			bool isSkinned = mesh.m_type == (int)MeshType::M_SKINNED ? true : false;
			std::wstring wname = mesh.m_szRes;
			std::string name(wname.begin(), wname.end());
			name += isSkinned ? " (Skinned)" : " (Static)";
			meshDisplayNames.push_back(name);
			meshDisplayNamePtrs.push_back(meshDisplayNames.back().c_str());
		}

		ImGui::Combo("Mesh List", &m_iSelectedMeshIndex, meshDisplayNamePtrs.data(), (int)meshDisplayNamePtrs.size());
		ImGui::InputText("Texture File##Root", m_szTextureName, IM_ARRAYSIZE(m_szTextureName));
		ImGui::InputText("Shader File##Root", m_szShaderName, IM_ARRAYSIZE(m_szShaderName));

		//// Animation List
		//if (!m_vAnimList.empty())
		//{
		//    auto animInstance = m_pLoader->m_vAnimInstanceList[0];
		//    const auto& animList = animInstance->GetAnimTrackList();
		//    static std::vector<std::string> animNames;
		//    static std::vector<const char*> animNamePtrs;
		//    animNames.clear();
		//    animNamePtrs.clear();
		//    for (const auto& anim : animList)
		//    {
		//        std::wstring wname = anim.m_szName;
		//        std::string name(wname.begin(), wname.end());
		//        animNames.push_back(name);
		//        animNamePtrs.push_back(animNames.back().c_str());
		//    }
		//    ImGui::Combo("Anim", &m_iSelectedAnimIndex, animNamePtrs.data(), (int)animNamePtrs.size());
		//    static float tempAnimPlayRate = 1.0f;
		//    if (tempAnimPlayRate == 1.0f)
		//        tempAnimPlayRate = animInstance->m_fAnimPlayRate;
		//    ImGui::SliderFloat("Anim Speed", &tempAnimPlayRate, 1.0f, 60.0f, "%.1f");
		//    animInstance->m_fAnimPlayRate = tempAnimPlayRate;
		//}

		if (ImGui::Button("Attach Mesh Component"))
		{
			/* m_pRootComponent = m_vMeshList[m_iSelectedMeshIndex];
			 if (m_pRootComponent)
			 {
				 m_pRootComponent->SetMeshPath(to_mw(m_szAssetPath));
				 auto mat = std::make_shared<UMaterial>();
				 mat->Load(to_mw(m_szTextureName), to_mw(m_szShaderName));
				 shared_ptr<Shader> shader = SHADER->Get(to_mw(m_szShaderName));
				 INPUTLAYOUT->CreateIW(shader->m_pCode);
				 mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
				 m_pRootComponent->SetMaterial(mat);
				 if (m_pLoader && !m_vAnimList.empty())
				 {
					 auto animInstance = m_pLoader->m_vAnimInstanceList[0];
					 if (auto skinnedMesh = std::dynamic_pointer_cast<USkinnedMeshComponent>(m_pRootComponent))
					 {
						 skinnedMesh->SetBaseAnim(animInstance);
						 auto animTrack = std::make_shared<AnimTrack>();
						 animTrack->SetBase(animInstance);
						 skinnedMesh->SetMeshAnim(animTrack);
						 animInstance->SetCurrentAnimTrack(m_iSelectedAnimIndex);
					 }
				 }
				 m_bRootSet = true;
			 }*/

			m_pRootComponent = meshLoader.Make(m_vMeshDataList[m_iSelectedMeshIndex]);
			//if (m_mAnimMap.size() > 0)
			//{
			//	auto originAnim = m_mAnimMap.find(m_vMeshDataList[m_iSelectedMeshIndex].m_szAnim);
			//	if (originAnim == m_mAnimMap.end()) { assert(false); }
			//	auto animInstance = originAnim->second->Clone();
			//}
			//m_pRootComponent = MakeMesh(m_vMeshDataList[m_iSelectedMeshIndex], true, animInstance);
			//auto& data = m_vMeshDataList[m_iSelectedMeshIndex];
			//for (int i = 0; i < data.m_vChild.size(); i++)
			//{
			//	m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false, animInstance));
			//}
		}
	}

	if (m_bRootSet)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Mesh component set.");
	}

#pragma region original

	//if (m_pRootComponent)
	//{
	//    ImGui::Separator();

	//    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Attach Child Mesh");

	//    if (!m_vMeshList.empty())
	//    {
	//        ImGui::Combo("Child Mesh", &m_iSelectedChildMeshIndex, meshDisplayNamePtrs.data(), (int)meshDisplayNamePtrs.size());
	//    }

	//    auto skinnedRoot = std::dynamic_pointer_cast<USkinnedMeshComponent>(m_pRootComponent);
	//    auto staticRoot = std::dynamic_pointer_cast<UStaticMeshComponent>(m_pRootComponent);

	//    if (skinnedRoot && skinnedRoot->GetMesh())
	//    {
	//        const auto& boneMap = skinnedRoot->GetMesh()->GetSkeletonList();
	//        static std::vector<std::string> boneNames;
	//        static std::vector<const char*> boneNamePtrs;

	//        boneNames.clear();
	//        boneNamePtrs.clear();

	//        for (auto it = boneMap.begin(); it != boneMap.end(); ++it)
	//        {
	//            const auto& boneNameW = it->first;
	//            const auto& boneNode = it->second;

	//            std::string boneName(boneNameW.begin(), boneNameW.end());
	//            boneNames.push_back(boneName);
	//            boneNamePtrs.push_back(boneNames.back().c_str());
	//        }

	//        static int selectedBoneIndex = 0;
	//        if (!boneNamePtrs.empty())
	//        {
	//            ImGui::Combo("Attach Bone", &selectedBoneIndex, boneNamePtrs.data(), (int)boneNamePtrs.size());
	//        }

	//        ImGui::InputText("Texture File##Child", m_szChildTextureName, IM_ARRAYSIZE(m_szChildTextureName));
	//        ImGui::InputText("Shader File##Child", m_szChildShaderName, IM_ARRAYSIZE(m_szChildShaderName));

	//        if (ImGui::Button("Attach To Root"))
	//        {
	//            auto parent = m_pRootComponent;
	//            auto childMesh = m_vMeshList[m_iSelectedChildMeshIndex];

	//            bool isSkinnedChild = std::dynamic_pointer_cast<USkinnedMeshComponent>(childMesh) != nullptr;

	//            if (isSkinnedChild)
	//            {
	//                auto skinnedChild = std::dynamic_pointer_cast<USkinnedMeshComponent>(childMesh);

	//                if (auto skinnedParent = std::dynamic_pointer_cast<USkinnedMeshComponent>(parent))
	//                {
	//                    auto mat = std::make_shared<UMaterial>();
	//                    mat->Load(to_mw(m_szChildTextureName), to_mw(m_szShaderName));
	//                    shared_ptr<Shader> shader = SHADER->Get(to_mw(m_szShaderName));
	//                    INPUTLAYOUT->CreateIW(shader->m_pCode);
	//                    mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
	//                    skinnedChild->SetMaterial(mat);

	//                    auto anim = skinnedParent->GetAnimInstance();
	//                    skinnedChild->SetBaseAnim(anim);
	//                }
	//                auto prevLastChild = m_pRootComponent->GetChildren().empty() ? nullptr : m_pRootComponent->GetChildren().back();
	//                parent->AddChild(skinnedChild);

	//                if (!m_pRootComponent->GetChildren().empty() && m_pRootComponent->GetChildren().back() != prevLastChild)
	//                {
	//                    auto lastChild = m_pRootComponent->GetChildren().back();
	//                    if (lastChild)
	//                    {
	//                        lastChild->SetMeshPath(to_mw(m_szAssetPath));
	//                        m_bChildAttached = true;
	//                    }
	//                }
	//            }
	//            else
	//            {
	//                auto staticChild = std::dynamic_pointer_cast<UStaticMeshComponent>(childMesh);

	//                if (auto skinnedParent = std::dynamic_pointer_cast<USkinnedMeshComponent>(parent))
	//                {
	//                    auto mat = std::make_shared<UMaterial>();
	//                    mat->Load(to_mw(m_szChildTextureName), to_mw(m_szShaderName));
	//                    staticChild->SetMaterial(mat);

	//                    auto anim = skinnedParent->GetAnimInstance();
	//                    staticChild->SetAnimInstance(anim);
	//                    staticChild->SetTargetBoneIndex(selectedBoneIndex);
	//                }
	//                auto prevLastChild = m_pRootComponent->GetChildren().empty() ? nullptr : m_pRootComponent->GetChildren().back();
	//                parent->AddChild(staticChild);

	//                if (!m_pRootComponent->GetChildren().empty() && m_pRootComponent->GetChildren().back() != prevLastChild)
	//                {
	//                    auto lastChild = m_pRootComponent->GetChildren().back();
	//                    if (lastChild)
	//                    {
	//                        lastChild->SetMeshPath(to_mw(m_szAssetPath));
	//                        m_bChildAttached = true;
	//                    }
	//                }
	//            }
	//        }
	//    }
	//    // *StaticRoot인 경우 없으면 삭제
	//    // *SkinnedChild 추가 로직 필요
	//    else if (staticRoot)
	//    {
	//        if (ImGui::Button("Attach To Root"))
	//        {
	//            auto child = m_vMeshList[m_iSelectedMeshIndex];
	//            auto prevLastChild = m_pRootComponent->GetChildren().empty() ? nullptr : m_pRootComponent->GetChildren().back();
	//            
	//            auto mat = std::make_shared<UMaterial>();
	//            mat->Load(to_mw(m_szChildTextureName), to_mw(m_szShaderName));
	//            child->SetMaterial(mat);

	//            staticRoot->AddChild(child);

	//            if (!m_pRootComponent->GetChildren().empty() && m_pRootComponent->GetChildren().back() != prevLastChild)
	//            {
	//                auto lastChild = m_pRootComponent->GetChildren().back();
	//                if (lastChild)
	//                {
	//                    lastChild->SetMeshPath(to_mw(m_szAssetPath));
	//                    m_bChildAttached = true;
	//                }
	//            }
	//        }
	//    }
	//}

	//if (m_bChildAttached)
	//{
	//    ImGui::TextColored(ImVec4(0, 1, 0, 1), "child attached to root.");
	//}
#pragma endregion

	ImGui::Separator();

	// SRT 적용 -> Callback 처리
	SetActor();

	ImGui::Separator();

	// Component 추가
	// Camera, Shape, Script ...
	SetComponent();

	// Actor 생성
	ImGui::Separator();
	if (ImGui::Button("Finalize Actor Creation"))
	{
		if (m_pRootComponent && m_OnCreate)
		{
			Vec3 position(m_fPosition[0], m_fPosition[1], m_fPosition[2]);
			Vec3 rotation(m_fRotation[0], m_fRotation[1], m_fRotation[2]);
			Vec3 scale(m_fScale[0], m_fScale[1], m_fScale[2]);

			m_OnCreate(m_pRootComponent, position, rotation, scale, m_CameraData, m_ShapeData, m_iSelectedScriptIndex);
		}

		m_bRootSet = false;
		m_bChildAttached = false;
	}

	ImGui::Separator(); ImGui::Spacing();

	// Prefab Save/Load
	SaveCharacterPrefab();

	ImGui::Spacing();

	LoadCharacterPrefab();

	ImGui::Spacing();

	// 프리팹 브라우저 : 현재 미구현
	BrowserSection();

	ImGui::Separator(); ImGui::Spacing();

	// 새로운 캐릭터 생성 시 초기화
	ResetButton();
}

void CharacterEditorUI::DrawVec3Slider(const char* label, float* values, float minVal, float maxVal)
{
	ImGui::PushID(label);
	ImGui::Text("%s", label);
	ImGui::Indent(10.0f);

	static const char* axes[] = { "X", "Y", "Z" };
	const float buttonSize = 20.0f;
	const float inputWidth = 60.0f;
	const float sliderWidth = 150.0f;
	const float spacing = 4.0f;

	for (int i = 0; i < 3; ++i)
	{
		ImGui::PushID(i);

		ImGui::Text("%s", axes[i]); ImGui::SameLine();

		if (ImGui::Button("-", ImVec2(buttonSize, 0)))
			values[i] -= 0.1f;
		ImGui::SameLine(0, spacing);

		ImGui::PushItemWidth(sliderWidth);
		if (ImGui::SliderFloat("##Slider", &values[i], minVal, maxVal))
		{
			// 값이 바뀐 경우 추가 작업 가능
		}
		ImGui::PopItemWidth();

		ImGui::SameLine(0, spacing);

		if (ImGui::Button("+", ImVec2(buttonSize, 0)))
			values[i] += 0.1f;

		ImGui::SameLine(0, spacing);

		ImGui::PushItemWidth(inputWidth);
		ImGui::InputFloat("##Value", &values[i], 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopID();
	}

	ImGui::Unindent(10.0f);
	ImGui::PopID();
}

void CharacterEditorUI::SetActor()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
	DrawVec3Slider("Position", m_fPosition, -100.f, 100.f);
	DrawVec3Slider("Rotation", m_fRotation, -360.f, 360.f);
	DrawVec3Slider("Scale", m_fScale, 0.01f, 10.f);
}

void CharacterEditorUI::SetComponent()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Set Component");

	static std::vector<std::string> componentNames 
		= { "None", "CameraComponent", "ShapeComponent" };
	static std::vector<const char*> componentNamePtrs;
	if (componentNamePtrs.empty())
	{
		for (auto& name : componentNames)
			componentNamePtrs.push_back(name.c_str());
	}

	ImGui::Combo("Component", &m_iSelectedComoponentIndex, componentNamePtrs.data(), (int)componentNamePtrs.size());
	ImGui::Separator();

	if (m_iSelectedComoponentIndex == 1)
	{
		m_CameraData.isUse = true;
	}
	else if (m_iSelectedComoponentIndex == 2)
	{
		m_ShapeData.isUse = true;
	}

	if (m_CameraData.isUse)
	{
		SetCameraComponent();
		ImGui::Separator();
	}

	if (m_ShapeData.isUse)
	{
		SetShapeComponent();
		ImGui::Separator();
	}

	// 스크립트 설정
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Script Configuration");

	// *여기에 스크립트 리스트 넣어야함(이름 정확히)
	static std::vector<std::string> scriptNames = { "None", "PlayerMoveScript", "EnemyAIScript" };
	static std::vector<const char*> scriptNamePtrs;
	if (scriptNamePtrs.empty())
	{
		for (auto& name : scriptNames)
			scriptNamePtrs.push_back(name.c_str());
	}

	ImGui::Combo("Script", &m_iSelectedScriptIndex, scriptNamePtrs.data(), (int)scriptNamePtrs.size());

}

void CharacterEditorUI::SetCameraComponent()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera Configuration");
	ImGui::Checkbox("Use Camera", &m_ShapeData.isUse);

	DrawVec3Slider("Camera Position", m_CameraData.Position, -100.f, 100.f);
	DrawVec3Slider("Camera Rotation", m_CameraData.Rotation, -360.f, 360.f);

	ImGui::PushID("CameraFov");
	ImGui::Text("%s", "Fov"); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::InputFloat("##Value", &m_CameraData.Fov);
	ImGui::PopItemWidth(); ImGui::SameLine();
	ImGui::PopID();

	ImGui::PushID("CameraAspect");
	ImGui::Text("%s", "Aspect"); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::InputFloat("##Value", &m_CameraData.Aspect);
	ImGui::PopItemWidth();
	ImGui::PopID();

	ImGui::PushID("CameraNear");
	ImGui::Text("%s", "Near"); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::InputFloat("##Value", &m_CameraData.Near);
	ImGui::PopItemWidth(); ImGui::SameLine();
	ImGui::PopID();

	ImGui::PushID("CameraFar");
	ImGui::Text("%s", "Far"); ImGui::SameLine();
	ImGui::PushItemWidth(60.f);
	ImGui::InputFloat("##Value", &m_CameraData.Far);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void CharacterEditorUI::SetShapeComponent()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Shape Configuration");
	ImGui::Checkbox("Use Shape", &m_ShapeData.isUse);

	static std::vector<std::string> shapeNames
		= { "Box", "Sphere" };
	static std::vector<const char*> shapeNamePtrs;
	if (shapeNamePtrs.empty())
	{
		for (auto& name : shapeNames)
			shapeNamePtrs.push_back(name.c_str());
	}

	static int iSelectedShapeIndex = 0;

	ImGui::Combo("ShapeType", &iSelectedShapeIndex, shapeNamePtrs.data(), (int)shapeNamePtrs.size());

	if (iSelectedShapeIndex == 0)
	{
		m_ShapeData.eShapeType = ShapeType::ST_BOX;
	}
	else if (iSelectedShapeIndex == 1)
	{
		m_ShapeData.eShapeType = ShapeType::ST_SPHERE;
	}

	DrawVec3Slider("Shape Position", m_ShapeData.Position, -100.f, 100.f);
	DrawVec3Slider("Shape Rotation", m_ShapeData.Rotation, -360.f, 360.f);
	DrawVec3Slider("Shape Scale", m_ShapeData.Scale, 0.01f, 10.f);
}

void CharacterEditorUI::SaveCharacterPrefab()
{
	ImGui::BeginChild("SaveCharacterSection", ImVec2(0, 100), true);
	{
		ImGui::Text("Save Character Prefab");
		static char characterSavePath[256] = "../Resources/Prefab/MyCharacter.character.json";
		ImGui::InputText("Save Path", characterSavePath, IM_ARRAYSIZE(characterSavePath));

		if (ImGui::Button("Save Character", ImVec2(-1, 0)))
		{
			PrefabCharacterData data;
			data.Name = "MyCharacter";
			data.RootMeshPath = m_szAssetPath;
			data.MeshPath = m_szMeshPath;
			data.ShaderPath = m_szShaderName;
			data.TexturePath = m_szTextureName;
			data.ScriptType = m_iSelectedScriptIndex;
			data.AnimIndex = m_iSelectedAnimIndex;

			auto animInstance = m_pLoader->m_vAnimInstanceList.empty() ? nullptr : m_pLoader->m_vAnimInstanceList[0];
			data.AnimSpeed = animInstance ? animInstance->m_fAnimPlayRate : 1.0f;

			for (int i = 0; i < 3; i++)	data.transform.Scale[i] = m_fScale[i];
			for (int i = 0; i < 3; i++)	data.transform.Rotation[i] = m_fRotation[i];
			for (int i = 0; i < 3; i++)	data.transform.Position[i] = m_fPosition[i];

			//for (int i = 0; i < m_iChildIndex; ++i)
			//{
			//    auto child = m_pRootComponent->GetChild(i);
			//    if (child)
			//    {
			//        PrefabCharacterData::ChildMeshData childData;
			//        childData.MeshPath = "Unknown";
			//        childData.TargetBoneIndex = m_iSelectedBoneIndex;
			//        data.ChildMeshes.push_back(childData);
			//    }
			//}		
				
			// Component
			data.camera = m_CameraData;
			data.shape = m_ShapeData;

			PREFAB->SaveCharacter(data, characterSavePath);
		}
	}
	ImGui::EndChild();
}

void CharacterEditorUI::LoadCharacterPrefab()
{
	ImGui::BeginChild("LoadCharacterSection", ImVec2(0, 100), true);
	{
		ImGui::Text("Load Character Prefab");
		static char characterLoadPath[256] = "../Resources/Prefab/MyCharacter.character.json";
		ImGui::InputText("Load Path", characterLoadPath, IM_ARRAYSIZE(characterLoadPath));

		if (ImGui::Button("Load Character", ImVec2(-1, 0)))
		{
			PrefabCharacterData data;
			if (PREFAB->LoadCharacter(characterLoadPath, data))
			{
				strcpy_s(m_szAssetPath, data.RootMeshPath.c_str());
				strcpy_s(m_szMeshPath, data.MeshPath.c_str());
				strcpy_s(m_szShaderName, data.ShaderPath.c_str());
				strcpy_s(m_szTextureName, data.TexturePath.c_str());

				m_iSelectedScriptIndex = data.ScriptType;
				m_iSelectedAnimIndex = data.AnimIndex;

				//auto animInstance = m_pLoader->m_vAnimInstanceList.empty() ? nullptr : m_pLoader->m_vAnimInstanceList[0];
				//if (animInstance)
				//{
				//    // *애니메이션 속도는 로더에서 따로 적용 필요
				//    animInstance->m_fAnimPlayRate = data.AnimSpeed;
				//}

				for (int i = 0; i < 3; i++) m_fScale[i] = data.transform.Scale[i];
				for (int i = 0; i < 3; i++) m_fRotation[i] = data.transform.Rotation[i];
				for (int i = 0; i < 3; i++) m_fPosition[i] = data.transform.Position[i];

				m_CameraData = data.camera;
				m_ShapeData = data.shape;

				Vec3 position(m_fPosition[0], m_fPosition[1], m_fPosition[2]);
				Vec3 rotation(m_fRotation[0], m_fRotation[1], m_fRotation[2]);
				Vec3 scale(m_fScale[0], m_fScale[1], m_fScale[2]);

				if (!m_pLoader) m_pLoader = std::make_shared<ActorLoader>();
				m_pLoader->LoadOne(m_szAssetPath);
				m_mMeshMap = m_pLoader->LoadMeshMap();
				m_mAnimMap = m_pLoader->LoadAnimMap();
				m_iSelectedMeshIndex = 0;
				meshLoader.SetMesh(m_mMeshMap);
				meshLoader.SetAnim(m_mAnimMap);

				m_pRootComponent = meshLoader.Make(m_szMeshPath);
				//auto meshData = AAsset::LoadMesh(m_szMeshPath);

				//auto originAnim = m_mAnimMap.find(meshData.m_szAnim);
				//if (originAnim == m_mAnimMap.end()) { assert(false); }
				//auto animInstance = originAnim->second->Clone();

				//m_pRootComponent = meshLoader.MakeMesh(meshData, true, animInstance);

				//auto& data = meshData;
				//for (int i = 0; i < data.m_vChild.size(); i++)
				//{
				//	m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false, animInstance));
				//}

				m_OnCreate(m_pRootComponent, position, rotation, scale, m_CameraData, m_ShapeData, m_iSelectedScriptIndex);
			}
		}
	}
	ImGui::EndChild();
}

void CharacterEditorUI::BrowserSection()
{
	// *프리팹 구조 확정되면 추가 기능 구현
	ImGui::BeginChild("PrefabBrowserSection", ImVec2(0, 150), true);
	{
		ImGui::Text("Prefab Browser");

		m_vCharacterPrefabList = PREFAB->GetPrefabFileNames("../Resources/Prefab/", ".character.json");
		static std::string selectedCharacterPrefab = "";

		for (const auto& name : m_vCharacterPrefabList)
		{
			if (ImGui::Selectable(name.c_str(), selectedCharacterPrefab == name))
				selectedCharacterPrefab = name;
		}

		if (!selectedCharacterPrefab.empty() && ImGui::Button("Load Selected Prefab", ImVec2(-1, 0)))
		{
			PrefabCharacterData data;
			std::string fullPath = "../Resources/Prefab/" + selectedCharacterPrefab + ".character.json";
			if (PREFAB->LoadCharacter(fullPath, data))
			{
				strcpy_s(m_szAssetPath, data.RootMeshPath.c_str());
				strcpy_s(m_szShaderName, data.ShaderPath.c_str());
				strcpy_s(m_szTextureName, data.TexturePath.c_str());

				m_iSelectedScriptIndex = data.ScriptType;
				m_iSelectedAnimIndex = data.AnimIndex;

				auto animInstance = m_pLoader->m_vAnimInstanceList.empty() ? nullptr : m_pLoader->m_vAnimInstanceList[0];
				if (animInstance)
					animInstance->m_fAnimPlayRate = data.AnimSpeed;

				for (int i = 0; i < 3; i++) m_fScale[i] = data.transform.Scale[i];
				for (int i = 0; i < 3; i++) m_fRotation[i] = data.transform.Rotation[i];
				for (int i = 0; i < 3; i++) m_fPosition[i] = data.transform.Position[i];

				if (!m_pLoader) m_pLoader = std::make_shared<ActorLoader>();
				m_pLoader->LoadOne(m_szAssetPath);
				m_vMeshList = m_pLoader->LoadMesh();
				m_iSelectedMeshIndex = 0;
			}
		}
	}
	ImGui::EndChild();
}

void CharacterEditorUI::ResetButton()
{
	if (ImGui::Button("Reset"))
	{
		m_pRootComponent = nullptr;

		m_iSelectedMeshIndex = 0;
		m_iSelectedChildMeshIndex = 0;
		m_iSelectedAnimIndex = 0;
		m_iSelectedComoponentIndex = 0;
		m_iSelectedScriptIndex = 0;

		m_iChildIndex = 0;

		m_CameraData.isUse = false;
		m_ShapeData.isUse = false;
	}
}

//shared_ptr<UMeshComponent> CharacterEditorUI::MakeMesh(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> animInstance)
//{
//	shared_ptr<UMeshComponent> mesh;
//	auto meshRes = m_mMeshMap.find(data.m_szRes);
//	if (meshRes == m_mMeshMap.end()) { assert(false); }
//
//
//	if (data.m_type == (int)MeshType::M_SKINNED)
//	{
//		mesh = make_shared<USkinnedMeshComponent>();
//		auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
//		skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshRes->second));
//
//		if (bRoot) { skinnedRoot->SetBaseAnim(animInstance); }
//		shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
//		animTrack->SetBase(animInstance);
//		skinnedRoot->SetMeshAnim(animTrack);
//		if (data.m_bInPlace)
//		{
//			animInstance->m_bInPlace = true;
//			animInstance->SetRootIndex(data.m_rootIndex);
//		}
//
//		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
//		mat->Load(data.m_szTex, data.m_szShader);
//		mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
//		skinnedRoot->SetMaterial(mat);
//	}
//	else
//	{
//		mesh = make_shared<UStaticMeshComponent>();
//		auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
//		staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(meshRes->second));
//
//		staticRoot->SetAnimInstance(animInstance);
//		staticRoot->SetMatBone(data.m_matBone);
//		staticRoot->SetTargetBoneIndex(data.m_targetBone);
//
//		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
//		mat->Load(data.m_szTex, data.m_szShader);
//		staticRoot->SetMaterial(mat);
//	}
//	mesh->SetLocalPosition(data.m_pos);
//	mesh->SetLocalRotation(data.m_rot);
//	mesh->SetLocalScale(data.m_scale);
//
//	return mesh;
//}
//
