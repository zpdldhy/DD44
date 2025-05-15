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

		// ROOT MESH
		ImGui::Combo("Mesh List", &m_meshIndex, m_vMeshPtrList.data(), (int)m_vMeshPtrList.size());
		ImGui::Combo("Anim List", &m_animIndex, m_vAnimPtrList.data(), (int)m_vAnimPtrList.size());
		ImGui::Combo("Texture List", &m_texIndex, m_vTexPtrList.data(), (int)m_vTexPtrList.size());

		{
			// Create Actor with Root Mesh
			if (ImGui::Button("Create Actor") && m_OnCreateActor)
			{
				m_OnCreateActor(m_meshIndex, m_animIndex, m_texIndex, m_pActor);
				SetBoneList(m_meshIndex);
				m_meshIndex = -1;
			}
		}
	}
	
	// CHILD
	if (m_pActor)
	{
		bool bMesh = ImGui::Combo("Mesh List", &m_meshIndex, m_vMeshPtrList.data(), (int)m_vMeshPtrList.size());
		bool bTex = ImGui::Combo("Texture List", &m_texIndex, m_vTexPtrList.data(), (int)m_vTexPtrList.size());
		
		m_childData.rootMesh = m_pActor;

		if (bMesh || m_childMeshSelected) 
		{ 
			m_childData.meshIndex = m_meshIndex;
			m_childMeshSelected = true;
			bool skinned = (dynamic_pointer_cast<USkinnedMeshComponent>(m_vMeshList[m_meshIndex]) != nullptr);
			m_childData.bSkeletal = skinned;
			if (!skinned)
			{
				m_childData.bAnimatedStatic = true;
				ImGui::Separator();
				bool bPBone = ImGui::Combo("Parent Bone List", &m_parentBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());
				bool bRBone = ImGui::Combo("Root Bone List", &m_rootBoneIndex, m_vBonePtrList.data(), (int)m_vBonePtrList.size());

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
		}
		
	}



}

void MeshEditorUI::SetMeshList(vector<shared_ptr<UMeshComponent>> _meshList)
{
	m_vMeshList = _meshList;
	m_vMeshNameList.reserve(_meshList.size());
	m_vMeshPtrList.reserve(_meshList.size());

	for (int iMesh = 0; iMesh < _meshList.size(); iMesh++)
	{
		std::string name = to_wm(_meshList[iMesh]->GetName());
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
void MeshEditorUI::SetBoneList(int _meshIndex)
{
	const auto& boneMap = dynamic_pointer_cast<USkinnedMeshComponent>(m_vMeshList[_meshIndex])->GetMesh()->GetSkeletonList();
	m_vBoneList.reserve(boneMap.size());
	m_vBonePtrList.reserve(boneMap.size());
	for (auto data : boneMap)
	{
		string name = to_wm(data.first);
		m_vBoneList.push_back(name);
		m_vBonePtrList.emplace_back(m_vBoneList.back().c_str());
	}
}