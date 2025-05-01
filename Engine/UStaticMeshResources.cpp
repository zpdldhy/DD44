#include "pch.h"
#include "UStaticMeshResources.h"

void UStaticMeshResources::Bind()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}
