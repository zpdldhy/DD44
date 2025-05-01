#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"

void TestYR::Init()
{

	//Gizmo 技泼
	{
		//shared_ptr<APawn> object1 = make_shared<APawn>();
		//shared_ptr<APawn> object2 = make_shared<APawn>();
		//shared_ptr<APawn> object3 = make_shared<APawn>();

		//// Material 积己
		//shared_ptr<UMaterial> material = make_shared<UMaterial>();
		//material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

		//// mesh 积己
		//shared_ptr<UStaticMeshComponent> mesh = make_shared<UStaticMeshComponent>();
		//mesh->CreateCube();
		//mesh->SetMaterial(material);

		//// Object 汲沥
		//object1->SetMesh(mesh);
		//object2->SetMesh(mesh);
		//object3->SetMesh(mesh);

		//object1->SetScale(Vec3(1000.0f, 0.03f, 0.03f));
		//object2->SetScale(Vec3(0.03f, 1000.0f, 0.03f));
		//object3->SetScale(Vec3(0.03f, 0.03f, 1000.0f));
		//
		//object1->Init();
		//object2->Init();
		//object3->Init();
		//
		//gizmo.emplace_back(object1);
		//gizmo.emplace_back(object2);
		//gizmo.emplace_back(object3);
	}

	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	m_pCameraActor->Init();

	m_vActorList = loader.Load();
	for (int i = 0; i < m_vActorList.size(); i++)
	{
		m_vActorList[i]->Init();
	}
}
void TestYR::Update()
{
	m_pCameraActor->Tick();
	//for (int i = 0; i < gizmo.size(); i++)
	//{
	//	gizmo[i]->Tick();
	//}
	for (int i = 0; i < m_vActorList.size(); i++)
	{
		m_vActorList[i]->Tick();
	}
}
void TestYR::Render()
{
	m_pCameraActor->Render();
	//for (int i = 0; i < gizmo.size(); i++)
	//{
	//	gizmo[i]->Render();
	//}
	for (int i = 0; i < m_vActorList.size(); i++)
	{
		m_vActorList[i]->Render();
	}
}