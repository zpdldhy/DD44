#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
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

		OBJECTMANAGER->AddActorList(gizmo);
	}
#pragma endregion
 
#pragma region 카메라 세팅
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	CAMERAMANAGER->Set3DCameraActor(m_pCameraActor);
	OBJECTMANAGER->AddActor(m_pCameraActor);
#pragma endregion

	//for (int i = 0; i < m_vActorList.size(); i++)
	//{
	//	m_vActorList[i]->SetPosition(Vec3(10.0f * i , 0, 0));
	//	OBJECTMANAGER->AddActor(m_vActorList[i]);
	//}


#pragma region HeadRollerTest
	{
		//		//loader.ConvertFbxToAsset();
		//
		//		m_pActor = loader.LoadOneActor("../Resources/Asset/_E_HEADROLLER.asset");
		//		meshList = loader.LoadMesh();
		//		targetObj = m_pActor;
		//		auto animInstance = targetObj->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		//#pragma region Child
		//		{
		//			Matrix matBone1 = animInstance->GetBoneAnim(33);
		//			matBone1 = matBone1.Invert();
		//			dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetMatBone(matBone1);
		//
		//			dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetAnimInstance(animInstance);
		//			dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetTargetBoneIndex(33);
		//			meshList[1]->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.5f));
		//			targetObj->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[1]);
		//
		//			Matrix matBone2 = animInstance->GetBoneAnim(34);
		//			matBone2 = matBone1.Invert();
		//			dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetMatBone(matBone1);
		//
		//			dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetAnimInstance(animInstance);
		//			dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetTargetBoneIndex(33);
		//			meshList[2]->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.5f));
		//			targetObj->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[2]);
		//
		//		}
		//#pragma endregion
		//
		//		OBJECTMANAGER->AddActor(m_pActor);
	}
#pragma endregion

#pragma region ActorTest
	{
		loader.ConvertFbxToAsset();
		loader.LoadOne("../Resources/Asset/DD_crow.asset");
		tempmeshList = loader.LoadMesh();
		animInstanceList = loader.LoadAnim();
		m_pActor = make_shared<APawn>();

		auto animInstance = animInstanceList[0];

		dynamic_cast<USkinnedMeshComponent*>(tempmeshList[0].get())->SetBaseAnim(animInstance);
		shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
		animTrack->SetBase(animInstance);
		dynamic_cast<USkinnedMeshComponent*>(tempmeshList[0].get())->SetMeshAnim(animTrack);

		m_pActor->SetMeshComponent(tempmeshList[0]);

#pragma region Child
		//{
		//	auto weapon = dynamic_cast<UStaticMeshComponent*>(tempmeshList[2].get());

		//	Matrix matBone = animInstance->GetBoneAnim(43);
		//	//matBone = matBone.Invert();
		//	weapon->SetMatBone(matBone);

		//	weapon->SetAnimInstance(animInstance);
		//	weapon->SetTargetBoneIndex(43);
		//	weapon->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.5f));
		//	m_pActor->GetMeshComponent<USkinnedMeshComponent>()->AddChild(tempmeshList[2]);
		//}
		//{
		//	auto eye = dynamic_cast<UStaticMeshComponent*>(tempmeshList[1].get());

		//	Matrix matBone = animInstance->GetBoneAnim(57);
		//	//matBone = matBone.Invert();
		//	eye->SetMatBone(matBone);

		//	eye->SetAnimInstance(animInstance);
		//	eye->SetTargetBoneIndex(57);
		//	// Idle
		//	eye->SetLocalPosition(Vec3(-0.45f, 0.35f, 0.27f));
		//	m_pActor->GetMeshComponent<USkinnedMeshComponent>()->AddChild(tempmeshList[1]);

		//}
		//{
		//	auto tail = dynamic_cast<USkinnedMeshComponent*>(tempmeshList[5].get());
		//	tail->SetMeshAnim(animTrack);
		//}
#pragma endregion

		OBJECTMANAGER->AddActor(m_pActor);
	}
#pragma endregion

#pragma region MageTest
	//		//loader.ConvertFbxToAsset();
	//
	//		m_pActor = loader.LoadOneActor("../Resources/Asset/_E_MAGE.asset");
	//		meshList = loader.LoadMesh();
	//		targetObj = m_pActor;
	//		auto animInstance = targetObj->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	//#pragma region Child
	//		{
	//			Matrix matBone1 = animInstance->GetBoneAnim(67);
	//			matBone1 = matBone1.Invert();
	//			dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetMatBone(matBone1);
	//
	//			dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetAnimInstance(animInstance);
	//			dynamic_cast<UStaticMeshComponent*>(meshList[1].get())->SetTargetBoneIndex(67);
	//			meshList[1]->SetLocalRotation(Vec3(0.0f, -90.0f, 0.0f));
	//			//meshList[1]->SetLocalPosition(Vec3(-1.15f, -2.0f, -0.5f));
	//			targetObj->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[1]);
	//
	//			//Matrix matBone2 = animInstance->GetBoneAnim(34);
	//			//matBone2 = matBone1.Invert();
	//			//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetMatBone(matBone1);
	//
	//			//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetAnimInstance(animInstance);
	//			//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetTargetBoneIndex(33);
	//			//meshList[2]->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.5f));
	//			//targetObj->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[2]);
	//
	//		}
	//#pragma endregion
	//
	//		OBJECTMANAGER->AddActor(m_pActor);

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

#pragma region Crow_door

	//loader.ConvertFbxToAsset();
	//loader.LoadOne("../Resources/Asset/CrowDoor.asset");
	//m_pActor = make_shared<APawn>();
	//vector<shared_ptr<UMeshComponent>> meshList = loader.LoadMesh();
	//vector<wstring> texList = loader.LoadTexPath();
	//wstring texPath = L"../Resources/Texture/";
	//texPath += texList[0];
	//shared_ptr<Texture> tex = TEXTURE->Get(texPath);
	//m_pActor->SetMeshComponent(meshList[0]);
	//meshList[0]->GetMaterial()->SetTexture(tex);
	//for (int i = 1; i < meshList.size(); i++)
	//{
	//	wstring texPath = L"../Resources/Texture/";
	//	texPath += texList[i];
	//	tex = TEXTURE->Get(texPath);
	//	meshList[i]->GetMaterial()->SetTexture(tex);

	//	meshList[0]->AddChild(meshList[i]);
	//}
	//OBJECTMANAGER->AddActor(m_pActor);

#pragma endregion
}
void TestYR::Update()
{
	if (INPUT->GetButton(LCLICK))
	{
		//int targetIndex = m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->GetAnimIndex(L"Slash_Light_R_new");
		//m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->PlayOnce(targetIndex);
	}

}
void TestYR::Render()
{
}


