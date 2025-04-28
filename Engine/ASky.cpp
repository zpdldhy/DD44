#include "pch.h"
#include "ASky.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "DxState.h"

void ASky::Init()
{
	SetScale(Vec3(1.0f, 1.0f, 1.0f));

	AActor::Init();
}

void ASky::Tick()
{
	AActor::Tick();
}

void ASky::Render()
{
	DC->RSSetState(STATE->m_pRSSolidNone.Get());

	AActor::Render();

	DC->RSSetState(STATE->m_pRSSolid.Get());
}

