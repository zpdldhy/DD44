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
	if (m_pCurrentRasterizer)
		m_pCurrentRasterizer.Reset();

	DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
	DC->RSSetState(STATE->m_pRSSolidNone.Get());

	APawn::Render();

	DC->RSSetState(m_pCurrentRasterizer.Get());

	m_pCurrentRasterizer.Reset();
}

