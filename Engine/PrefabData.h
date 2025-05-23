#pragma once
#include "EditorData.h"

struct PrefabData
{
    std::string Name;
    std::string ActorType;

    std::string MeshPath;
    std::string ShaderPath;
    std::string TexturePath;

    Vec3 Scale;
    Vec3 Rotation;
    Vec3 Translation;
};

struct PrefabObjectData
{
    std::string Name;

    std::string MeshPath;
    std::string ShaderPath;
    std::string TexturePath;

    Vec3 Scale;
    Vec3 Rotation;
    Vec3 Translation;

    Vec3 SpecularColor;
    float Shininess;

    Vec3 EmissiveColor;
    float EmissivePower;

    ShapeComponentData ShapeData;
};

struct PrefabCharacterData
{
    std::string Name;

    std::string RootMeshPath;
    std::string MeshPath;
    std::string ShaderPath;
    std::string TexturePath;

    int ScriptType;

    int AnimIndex;
    float AnimSpeed;

    TransformData transform;

    // Component
	CameraComponentData camera;
	ShapeComponentData shape;

    struct ChildMeshData
    {
        std::string MeshPath;
        int TargetBoneIndex;
    };
    std::vector<ChildMeshData> ChildMeshes;

};

struct PrefabUIData
{
    std::string Name;

    TransformData transform;

	float SliceUV[4];

    std::string IdleTexturePath;
    std::string HoverTexturePath;
    std::string ActiveTexturePath;
    std::string SelectedTexturePath;
    std::string ShaderPath;

    int ScriptType;
};

struct PrefabParticleData
{
    std::string Name;

    std::string ShaderPath;
    std::string TexturePath;

    Vec3 Scale;
    Vec3 Rotation;
    Vec3 Translation;

    TransformData actor;

    int Divisions = 4;     // NxN
    int Row = 0;
    int Col = 0;

    Vec2 UVStart = { 0.0f, 0.0f };
    Vec2 UVEnd = { 1.0f, 1.0f };

    float BillboardSizeX = 100.0f;
    float BillboardSizeY = 100.0f;

    float Duration = 1.0f;
    bool bLoop = true;
    bool bAutoDestroy = false;
};

struct PrefabParticleGroupData
{
    std::string GroupName; 
    std::vector<PrefabParticleData> Particles;
};

struct ModifiedCellData
{
    int Row;
    int Col;
    float Height;
};

struct PrefabMapData
{
    int Cols = 0;
    int Rows = 0;
    float CellSize = 0.0f;

    Vec3 Position;
    Vec3 Rotation;
    Vec3 Scale;

    int SelectedRow = -1;
    int SelectedCol = -1;
    float TargetHeight = 0.0f;

    std::string TexturePath;
    std::string ShaderPath;
};