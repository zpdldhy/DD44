#include "pch.h"
#include "MeshEditorUI.h"
#include "UMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "APawn.h"

void MeshEditorUI::DrawUI()
{
	// ROOT MESH
	if(m_pActor == nullptr)
	{
		ImGui::Text("Set Root Mesh");

		ImGui::Combo("Mesh List", &m_meshIndex, m_vMeshPtrList.data(), (int)m_vMeshPtrList.size());
		if (m_meshIndex > -1)
		{
			m_baseMeshSelected = true;
			wstring name = to_mw(m_vMeshPtrList[m_meshIndex]);
			SetBoneList(name);
		}
		ImGui::Combo("Anim List", &m_animIndex, m_vAnimPtrList.data(), (int)m_vAnimPtrList.size());
		ImGui::Combo("Texture List", &m_texIndex, m_vTexPtrList.data(), (int)m_vTexPtrList.size());
		bool bRBone = ImGui::Combo("Root Bone List", &m_rootBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
		{
			// Create Actor with Root Mesh
			if (ImGui::Button("Create Actor") && m_OnCreateActor)
			{
				wstring mesh = to_mw(m_vMeshPtrList[m_meshIndex]);
				m_OnCreateActor(mesh, m_animIndex, m_texIndex, m_rootBoneIndex, m_pActor );
				m_meshIndex = m_rootBoneIndex  = -1;
			}
		}
	}
	
	// CHILD
	if (m_pActor)
	{
		bool bMesh = ImGui::Combo("Mesh List", &m_meshIndex, m_vMeshPtrList.data(), (int)m_vMeshPtrList.size());
		bool bTex = ImGui::Combo("Texture List", &m_texIndex, m_vTexPtrList.data(), (int)m_vTexPtrList.size());
		ImGui::InputText("Texture path", texPath, IM_ARRAYSIZE(texPath));

		m_childData.rootMesh = m_pActor;

		if (m_meshIndex >= 0)
		{ 
			m_childData.meshName = to_mw(m_vMeshPtrList[m_meshIndex]);
			m_childData.texPath = texPath;
			m_childMeshSelected = true;
			auto iter = m_mMeshResMap.find(m_childData.meshName);
			if (iter == m_mMeshResMap.end()) { assert(false); return; }
			int skinned = iter->second->GetType();
			m_childData.bSkeletal = (skinned == (int)(MeshType::M_SKINNED)) ? true : false ;
			m_childData.bAnimatedStatic = !m_childData.bSkeletal;
			if (!m_childData.bSkeletal)
			{
				ImGui::Separator();
				bool bPBone = ImGui::Combo("Parent Bone List", &m_parentBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
				bool bRBone = ImGui::Combo("Root Bone List", &m_rootBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
				if(ImGui::Checkbox("Inverse Mat Bone", &m_inverseMatBone))
				{
					m_childData.bInverseMatBone = m_inverseMatBone;
				}
			}
		}

		if (ImGui::Button("Create Child") && m_OnCreateChild)
		{
			m_childData.texIndex = m_texIndex;
			if (m_childData.bAnimatedStatic)
			{
				m_childData.parentBoneName = m_vBoneList[m_parentBoneIndex];
			}
			if (m_rootBoneIndex >= 0)
			{
				m_childData.bRootMotion = true;
				m_childData.rootBoneName = m_vBoneList[m_rootBoneIndex];
			}

			m_OnCreateChild(m_childData);
			m_vChildMeshPtrList.emplace_back(m_vMeshNameList[m_meshIndex].c_str());
			m_meshIndex = m_animIndex = m_parentBoneIndex = m_rootBoneIndex = -1;
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
		if(childChange) { m_parentBoneIndex = -1; }
		bool changeBone = ImGui::Combo("Parent Bone", &m_parentBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
		ImGui::Checkbox("InverseBone", &m_inverseMatBone);

		bool changeX = ImGui::DragFloat("x", &m_pos.x, -0.2f, +0.2f);
		bool changeY = ImGui::DragFloat("y", &m_pos.y, -0.2f, +0.2f);
		bool changeZ = ImGui::DragFloat("z", &m_pos.z, -0.2f, +0.2f);

		if (changeX || changeY || changeZ)
		{
			m_OnMoveChild(m_childIndex, m_pos);
		}

		if (changeBone)
		{
			m_OnParentBoneChange(m_childIndex, m_vBoneList[m_parentBoneIndex], m_inverseMatBone);
		}
	}



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

	// 위치를 다시 찾아야함
	//SetBoneList(m_meshIndex);
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
}