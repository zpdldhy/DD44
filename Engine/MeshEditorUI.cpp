#include "pch.h"
#include "MeshEditorUI.h"
#include "UMeshComponent.h"
#include "UMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "APawn.h"

void MeshEditorUI::DrawUI()
{
	if (m_pActor)
	{
		DrawMeshHierarchy();
	}

	if (m_vMeshPtrList.size() <= 0) { return; }

#pragma region RootMesh
	if (m_pActor == nullptr)
	{
		ImGui::Text("Root Mesh");
		// RootMesh 세팅 전
		if (!m_bRootCreated)
		{
			ImGui::Text("Mesh");
			bool meshSelected = ImGui::Combo("Mesh List", &m_meshIndex, m_vMeshPtrList.data(), (int)m_vMeshPtrList.size());
			ImGui::Separator();

			ImGui::Text("Material");
			DrawMatUI();

			ImGui::InputText("name of Comp", compName, sizeof(compName));
			// RootMesh 선택되었을 때 
			if (m_meshIndex > -1 && meshSelected)
			{
				wstring name = to_mw(m_vMeshPtrList[m_meshIndex]);
				auto iter = m_mMeshResMap.find(name);
				if (iter == m_mMeshResMap.end()) { assert(false); }
				m_bSkinnedRoot = iter->second->GetType() == (int)MeshType::M_SKINNED;
				if (m_bSkinnedRoot) { SetBoneList(name); }
			}

			// RootMesh == Skinned
			if (m_bSkinnedRoot)
			{
				ImGui::Combo("Anim List", &m_animIndex, m_vAnimPtrList.data(), (int)m_vAnimPtrList.size());
				ImGui::Combo("Root Bone List", &m_rootBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
			}

			// RootMesh로 actor 생성
			if (ImGui::Button("Create Actor") && m_OnCreateActor)
			{
				wstring meshName = to_mw(m_vMeshPtrList[m_meshIndex]);
				// Base
				PreMeshData root;
				root.bRoot = true;
				root.meshName = meshName;
				root.bSkeletal = m_bSkinnedRoot;
				root.compName = to_mw(compName);
				m_rootMeshName = compName;
				// Mat
				root.texIndex = m_texIndex;
				if (strcmp(texPath, "") != 0) { root.texPath = texPath; }
				if (strcmp(shaderPath, "") != 0) { root.shaderPath = shaderPath; }
				root.specular = m_specular;
				root.shininess = m_shininess;
				// Anim
				root.animIndex = m_animIndex;
				if (m_rootBoneIndex >= 0)
				{
					root.bRootMotion = true;
					root.rootBoneName = m_vBonePtrList[m_rootBoneIndex];
				}
				if (m_parentBoneIndex >= 0)
				{
					root.parentBoneName = m_vBonePtrList[m_parentBoneIndex];
				}
				m_OnCreateActor(root, m_pActor);

				// 멤버 변수 초기화
				m_meshIndex = -1;
				strcpy_s(compName, "");
			}
		}
	}
#pragma endregion

	// CHILD
	if (m_selectedMeshName.empty())
	{
		m_selectedMeshName = m_rootMeshName;
	}

	if (!m_selectedMeshName.empty())
	{
		bool bMesh = ImGui::Combo("Mesh List", &m_meshIndex, m_vMeshPtrList.data(), (int)m_vMeshPtrList.size());

		ImGui::InputText("name of Comp", compName, sizeof(compName));
		ImGui::Checkbox("Animated Static", &m_bAnimatedStatic);

		DrawMatUI();
		

		if (m_meshIndex >= 0)
		{
			m_childData.meshName = to_mw(m_vMeshPtrList[m_meshIndex]);
			auto iter = m_mMeshResMap.find(m_childData.meshName);
			if (iter == m_mMeshResMap.end()) { assert(false); return; }
			int skinned = iter->second->GetType();
			m_childData.bSkeletal = (skinned == (int)(MeshType::M_SKINNED)) ? true : false;
			m_childData.bAnimatedStatic = m_bAnimatedStatic;
			if (!m_childData.bSkeletal)
			{
				ImGui::Separator();
				bool bPBone = ImGui::Combo("Parent Bone List", &m_parentBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
				bool bRBone = ImGui::Combo("Root Bone List", &m_rootBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
				if (ImGui::Checkbox("Inverse Mat Bone", &m_inverseMatBone))
				{
					m_childData.bInverseMatBone = m_inverseMatBone;
				}
			}
		}

		if (ImGui::Button("Create Child") && m_OnCreateChild)
		{
			m_childData.actor = m_pActor;
			if (strcmp(texPath, "") == 0)
			{
				strcpy_s(texPath, m_vTexPtrList[m_texIndex]);
			}
			m_childData.texPath = texPath;
			if (strcmp(shaderPath, "") != 0)
			{
				m_childData.shaderPath = shaderPath;
			}
			m_childData.specular = m_specular;
			m_childData.shininess = m_shininess;

			m_childData.parentCompName = to_mw(m_selectedMeshName);
			m_childData.texIndex = m_texIndex;

			m_childData.compName = to_mw(compName);
			if (m_childData.bAnimatedStatic && m_parentBoneIndex >= 0)
			{
				m_childData.parentBoneName = m_vBoneList[m_parentBoneIndex];
			}
			if (m_rootBoneIndex >= 0)
			{
				m_childData.bRootMotion = true;
				m_childData.rootBoneName = m_vBoneList[m_rootBoneIndex];
			}
			m_childData.bVisible = m_bVisible;

			m_OnCreateChild(m_childData);
			m_vChildMeshPtrList.emplace_back(m_vMeshNameList[m_meshIndex].c_str());
			//m_meshIndex = m_animIndex = m_parentBoneIndex = m_rootBoneIndex = -1;
			m_modifyChild = true;
		}

		if (ImGui::Checkbox("AnimStop", &m_animStop) && m_OnAnimStop)
		{
			m_OnAnimStop(m_animStop);
		}



		bool bAnimChange = ImGui::Combo("Play Animation", &m_currentAnim, m_vAnimTrackPtrList.data(), (int)m_vAnimTrackPtrList.size());
		if (bAnimChange && m_OnAnimChange)
		{
			m_OnAnimChange(m_currentAnim);
		}

	}

	if (m_modifyChild)
	{
		ImGui::Separator();

		ImGui::Text("Child Position");
		bool childChange = ImGui::Combo("Child List", &m_childIndex, m_vChildMeshPtrList.data(), (int)m_vChildMeshPtrList.size());
		if (childChange) { m_parentBoneIndex = -1; }
		bool changeBone = ImGui::Combo("Parent Bone", &m_parentBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
		ImGui::Checkbox("InverseBone", &m_inverseMatBone);


		bool changeX = ImGui::DragFloat("pos x", &m_pos.x, -0.01f, +0.01f);
		bool changeY = ImGui::DragFloat("pos y", &m_pos.y, -0.01f, +0.01f);
		bool changeZ = ImGui::DragFloat("pos z", &m_pos.z, -0.01f, +0.01f);

		bool scaleX = ImGui::DragFloat("scale x", &m_scale.x, -0.01f, +0.01f);
		bool scaleY = ImGui::DragFloat("scale y", &m_scale.y, -0.01f, +0.01f);
		bool scaleZ = ImGui::DragFloat("scale z", &m_scale.z, -0.01f, +0.01f);
		bool scale = scaleX || scaleY || scaleZ;

		if (changeX || changeY || changeZ)
		{
			m_OnMoveChild(m_childIndex, m_pos);
		}

		if (scale)
		{
			m_OnScaleChild(m_childIndex, m_scale);
		}

		if (changeBone)
		{
			m_OnParentBoneChange(m_childIndex, m_vBoneList[m_parentBoneIndex], m_inverseMatBone);
		}

		if (ImGui::Checkbox("MeshVisible", &m_bVisible) && m_OnAnimStop)
		{
			m_OnChangeMeshVisible(m_childIndex, m_bVisible);
			//m_bVisible = true;
		}
	}

	ImGui::Separator();
	ImGui::Text("SAVE");
	ImGui::InputText("MeshFile Name", m_meshSaveName, IM_ARRAYSIZE(m_meshSaveName));
	if (ImGui::Button("Save mesh") && m_OnMeshSave)
	{
		bool nameSet = strcmp(m_meshSaveName, "");
		if (nameSet)
		{
			m_OnMeshSave(1, true, m_meshSaveName);

		}
	}

}

void MeshEditorUI::DrawMatUI()
{
	ImGui::Combo("Texture List", &m_texIndex, m_vTexPtrList.data(), (int)m_vTexPtrList.size());
	ImGui::InputText("Texture path", texPath, IM_ARRAYSIZE(texPath));
	ImGui::InputText("Shader path", shaderPath, IM_ARRAYSIZE(shaderPath));

	ImGui::InputFloat("Specular X", &m_specular.x);
	ImGui::InputFloat("Specular Y", &m_specular.y);
	ImGui::InputFloat("Specular Z", &m_specular.z);
	ImGui::InputFloat("Shininess", &m_shininess);
}

void MeshEditorUI::DrawMeshHierarchy()
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	ImGui::Begin("Actor Hierarchy");
	// 현재 actor를 돌면서 Mesh Hierarchy 보이기
	DrawMeshRecursive(m_pActor->GetMeshComponent());
	ImGui::End();
}
void MeshEditorUI::DrawMeshRecursive(shared_ptr<UMeshComponent> _child)
{
	ImGui::Indent();
	bool cliked = ImGui::Button(to_wm(_child->GetName()).c_str());
	if (cliked)
	{
		// Process Selected Mesh
		m_selectedMeshName = to_wm(_child->GetName());

	}
	// child 추가되는 부분이 여 아래 For문 도는 부분이랑 충돌 생길 수도 잇는 거 아냐 ? 
	for (auto& child : _child->GetChildren())
	{
		DrawMeshRecursive(child);
	}
	ImGui::Unindent();
}

void MeshEditorUI::SetMeshList(map<wstring, shared_ptr<UMeshResources>> _meshList)
{
	m_mMeshResMap = _meshList;
	m_vMeshNameList.reserve(_meshList.size());
	m_vMeshPtrList.reserve(_meshList.size());

	for (auto data : m_mMeshResMap)
	{
		std::string name = to_wm(data.first);
		m_vMeshNameList.push_back(name);
		m_vMeshPtrList.push_back(m_vMeshNameList.back().c_str());
	}
}
void MeshEditorUI::SetAnimList(vector<shared_ptr<UAnimInstance>> _animList)
{
	m_vAnimList.reserve(_animList.size());
	m_vAnimPtrList.reserve(_animList.size());

	for (int i = 0; i < _animList.size(); i++)
	{
		std::string name = to_wm(_animList[i]->GetName());
		m_vAnimList.push_back(name);
		m_vAnimPtrList.push_back(m_vAnimList.back().c_str());

		// 확장성 없음 .. 한 actor 별로만 작업 가능
		auto animList = _animList[i]->GetAnimTrackList();
		m_vAnimTrackList.reserve(animList.size());
		m_vAnimTrackPtrList.reserve(animList.size());

		for (int j = 0; j < animList.size(); j++)
		{
			std::string name = to_wm(animList[j].m_szName);
			m_vAnimTrackList.push_back(name);
			m_vAnimTrackPtrList.push_back(m_vAnimTrackList.back().c_str());
		}
	}
}
void MeshEditorUI::SetTexList(vector<wstring> _texureList)
{
	m_vTexList.reserve(_texureList.size());
	m_vTexPtrList.reserve(_texureList.size());

	for (int i = 0; i < _texureList.size(); i++)
	{
		std::string name = to_wm(_texureList[i]);
		m_vTexList.push_back(name);
		m_vTexPtrList.push_back(m_vTexList.back().c_str());

	}
}
void MeshEditorUI::SetBoneList(wstring meshName)
{
	auto iter = m_mMeshResMap.find(meshName);
	if (iter == m_mMeshResMap.end())
	{
		assert(false);
	}
	const auto& boneMap = dynamic_pointer_cast<USkeletalMeshResources>(iter->second)->GetSkeletonList();
	m_vBoneList.reserve(boneMap.size());
	m_vBonePtrList.reserve(boneMap.size());
	for (auto data : boneMap)
	{
		string name = to_wm(data.first);
		m_vBoneList.push_back(name);
		m_vBonePtrList.emplace_back(m_vBoneList.back().c_str());
	}
	m_bBoneSetting = true;
}
void MeshEditorUI::SetActor(shared_ptr<APawn>& _actor)
{
	m_pActor = _actor;
	//auto rootMesh = m_pActor->GetMeshComponent<USkinnedMeshComponent>();
	//// 자식들 돌면서 mesh setting
	//m_vMeshNameList.reserve(rootMesh->GetChildren().size());
	//m_vChildMeshPtrList.reserve(rootMesh->GetChildren().size());
	//for (auto& child : rootMesh->GetChildren())
	//{
	//	string name = to_wm(child->GetName());
	//	m_vMeshNameList.push_back(name);
	//	m_vChildMeshPtrList.emplace_back(m_vMeshNameList.back().c_str());
	//}

	//// bone은 rootmesh에서 가져와서
	//const auto& boneMap = dynamic_pointer_cast<USkeletalMeshResources>(rootMesh->GetMesh())->GetSkeletonList();
	//m_vBoneList.reserve(boneMap.size());
	//m_vBonePtrList.reserve(boneMap.size());
	//for (auto data : boneMap)
	//{
	//	string name = to_wm(data.first);
	//	m_vBoneList.push_back(name);
	//	m_vBonePtrList.emplace_back(m_vBoneList.back().c_str());
	//}
	//m_bBoneSetting = true;
	//// anim은 rootmesh에서 가져와서
	//// 걍 특정 키 누르면 


	//m_bActorSet = true;
}
