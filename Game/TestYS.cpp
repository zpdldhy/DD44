#include "pch.h"
#include "TestYS.h"

void TestYS::Init()
{
	// Initialize the camera
	{
		m_pCamera = make_shared<CameraActor>();
	}

	// Initialize the object
	{
		m_pObject = make_shared<AActor>();
		m_pObject->SetPosition(Vec3(0.0f, 0.0f, 0.0f));
		m_pObject->SetScale(Vec3(1.0f, 1.0f, 1.0f));

		// Set the Mesh
		auto mesh = make_shared<UStaticMeshComponent>();

		m_pObject->SetMesh(mesh);

	}
}

void TestYS::Update()
{
}

void TestYS::Render()
{
}
