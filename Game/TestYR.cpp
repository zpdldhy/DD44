#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "ObjectLoader.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "ObjectManager.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "Input.h"
#include "UMaterial.h"
#include "UAnimInstance.h"
#include "AnimTrack.h"
#include "ImGuiCore.h"
#include "UIManager.h"
#include "AAsset.h"
#include "SkeletalMeshData.h"
#include "Inputlayout.h"

void TestYR::Init()
{
#pragma region Gizmo 세팅
	{
		shared_ptr<APawn> object1 = make_shared<APawn>();
		shared_ptr<APawn> object2 = make_shared<APawn>();
		shared_ptr<APawn> object3 = make_shared<APawn>();

		// Material 생성
		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

		// mesh 생성
		shared_ptr<UStaticMeshComponent> meshComponent1 = UStaticMeshComponent::CreateCube();
		shared_ptr<UStaticMeshComponent> meshComponent2 = UStaticMeshComponent::CreateCube();
		shared_ptr<UStaticMeshComponent> meshComponent3 = UStaticMeshComponent::CreateCube();
		meshComponent1->SetMaterial(material);
		meshComponent2->SetMaterial(material);
		meshComponent3->SetMaterial(material);

		// Object 설정
		object1->SetMeshComponent(meshComponent1);
		object2->SetMeshComponent(meshComponent2);
		object3->SetMeshComponent(meshComponent3);

		object1->SetScale(Vec3(10000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 10000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 10000.0f));

		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);

		OBJECT->AddActorList(gizmo);
	}
#pragma endregion
 
#pragma region 카메라 세팅
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
#pragma endregion

#pragma region ActorTest
	{
		//loader.ConvertFbxToAsset();
		loader.LoadAllAsset();
		meshMap = loader.LoadMeshMap();
		tempmeshList = loader.LoadMesh();
		animInstanceList = loader.LoadAnim();
		animMap = loader.LoadAnimMap();
		m_pActor = make_shared<APawn>();

		shared_ptr<ObjectLoader>objLoader = make_shared< ObjectLoader>();
		objLoader->Load();
		map<wstring, shared_ptr<UMeshResources>> tempMap = objLoader->LoadMeshMap();

		for (auto& data : tempMap)
		{
			meshMap.insert(data);
		}
	
		// mesh export 할 시 name 설정 필요
		//AAsset::ExportMesh(m_pActor, "crow_final");
		MeshComponentData data = AAsset::LoadMesh("../Resources/Asset/crow_final.mesh");
		
		{
			auto originAnim = animMap.find(data.m_szAnim);
			if (originAnim == animMap.end()) { assert(false); }
			auto animInstance = originAnim->second->Clone();
			shared_ptr<UMeshComponent> root = MakeMC(data, true, animInstance);
			m_pActor = make_shared<APawn>();
			m_pActor->SetMeshComponent(root);

			for (int i = 0; i < data.m_vChild.size(); i++)
			{
				root->AddChild(MakeMC(data.m_vChild[i], false, animInstance));
			}
			OBJECT->AddActor(m_pActor);

		}

	}
#pragma endregion

#pragma region KnightTest

//	//loader.ConvertFbxToAsset();
//
//	loader.LoadOne("../Resources/Asset/_E_KNIGHT.asset");
//	meshList = loader.LoadMesh();
//	animInstanceList = loader.LoadAnim();
//
//	m_pActor = make_shared<APawn>();
//
//	auto animInstance = animInstanceList[0];
//	dynamic_cast<USkinnedMeshComponent*>(meshList[0].get())->SetBaseAnim(animInstance);
//	shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
//	animTrack->SetBase(animInstance);
//	dynamic_cast<USkinnedMeshComponent*>(meshList[0].get())->SetMeshAnim(animTrack);
//
//	m_pActor->SetMeshComponent(meshList[0]);
//#pragma region Child
//	{
//		for (int i = 1; i < meshList.size(); i++)
//		{
//			bool isSkinned = (dynamic_pointer_cast<USkinnedMeshComponent>(meshList[i]) != nullptr);
//			if(isSkinned)
//			{	
//				dynamic_cast<USkinnedMeshComponent*>(meshList[i].get())->SetMeshAnim(animTrack);
//
//			}
//			else
//			{
//				dynamic_cast<UStaticMeshComponent*>(meshList[i].get())->SetAnimInstance(animInstance);
//
//			}
//			meshList[0]->AddChild(meshList[i]);
//		}
//
//		//Matrix matBone1 = animInstance->GetBoneAnim(67);
//		//matBone1 = matBone1.Invert();
//		//dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetMatBone(matBone1);
//
//		//dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetAnimInstance(animInstance);
//		//dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetTargetBoneIndex(67);
//		//meshList[1]->SetLocalRotation(Vec3(0.0f, -90.0f, 0.0f));
//		////meshList[1]->SetLocalPosition(Vec3(-1.15f, -2.0f, -0.5f));
//		//targetObj->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[1]);
//
//		//Matrix matBone2 = animInstance->GetBoneAnim(34);
//		//matBone2 = matBone1.Invert();
//		//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetMatBone(matBone1);
//
//		//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetAnimInstance(animInstance);
//		//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetTargetBoneIndex(33);
//		//meshList[2]->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.5f));
//		//targetObj->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[2]);
//
//	}
//#pragma endregion
//
//	OBJECTMANAGER->AddActor(m_pActor);

#pragma endregion
}
void TestYR::Update()
{	
	// 애니메이션 확인
	if (INPUT->GetButton(LCLICK))
	{
		//int targetIndex = m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->GetAnimIndex(L"Slash_Light_R_new");
		//m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->PlayOnce(targetIndex);
	}

	if (INPUT->GetButton(C))
	{
		auto anim = m_pActor2->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		if (++targetIndex >= anim->GetAnimTrackList().size())
		{
			targetIndex = 0;
		}
		anim->SetCurrentAnimTrack(targetIndex);
	}

}
void TestYR::Render()
{
}

shared_ptr<UMeshComponent> TestYR::MakeMC(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> animInstance)
{
	shared_ptr<UMeshComponent> mesh;
	auto meshRes = meshMap.find(data.m_szRes);
	if (meshRes == meshMap.end()) { assert(false); }

	if (data.m_type == (int)MeshType::M_SKINNED)
	{
		mesh = make_shared<USkinnedMeshComponent>();
		auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshRes->second));

		if (bRoot) { skinnedRoot->SetBaseAnim(animInstance); }
		shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
		animTrack->SetBase(animInstance);
		skinnedRoot->SetMeshAnim(animTrack);
		if (data.m_bInPlace)
		{
			animInstance->m_bInPlace = true;
			animInstance->SetRootIndex(data.m_rootIndex);
		} 

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
		skinnedRoot->SetMaterial(mat);
	}
	else
	{
		mesh = make_shared<UStaticMeshComponent>();
		auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
		staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(meshRes->second));

		staticRoot->SetAnimInstance(animInstance);
		staticRoot->SetMatBone(data.m_matBone);
		staticRoot->SetTargetBoneIndex(data.m_targetBone);

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		staticRoot->SetMaterial(mat);
	}
	mesh->SetLocalPosition(data.m_pos);
	mesh->SetLocalRotation(data.m_rot);
	mesh->SetLocalScale(data.m_scale);

	return mesh;
}


