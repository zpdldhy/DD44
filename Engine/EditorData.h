#pragma once
#include "ShapeData.h"
#include "UShapeComponent.h"

struct TransformData
{
	// Transform
	float Position[3] = { 0.0f, 0.0f, 0.0f };
	float Rotation[3] = { 0.0f, 0.0f, 0.0f };
	float Scale[3] = { 1.0f, 1.0f, 1.0f };
};

struct CameraComponentData
{
	bool isUse = false;

	float Position[3] = { 0.0f, 0.0f, 0.0f };
	float Rotation[3] = { 0.0f, 0.0f, 0.0f };

	float Fov = 3.14f / 4.0f;
	float Aspect = static_cast<float>(g_windowSize.x) / static_cast<float>(g_windowSize.y);
	float Near = 0.1f;
	float Far = 5000.0f;
};

struct ShapeComponentData
{
	bool isUse = false;

	ShapeType eShapeType = ShapeType::ST_NONE;

	float Position[3] = { 0.0f, 0.0f, 0.0f };
	float Rotation[3] = { 0.0f, 0.0f, 0.0f };
	float Scale[3] = { 1.0f, 1.0f, 1.0f };
};