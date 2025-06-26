#pragma once
#include "EditorData.h"

//struct PrefabData
//{
//    std::string Name;
//    std::string ActorType;
//
//    std::string MeshPath;
//    std::string ShaderPath;
//    std::string TexturePath;
//
//    Vec3 Scale;
//    Vec3 Rotation;
//    Vec3 Position;
//};

struct PrefabObjectData
{
    std::string Name = "";
    UINT m_eActorType = 0;

    std::string MeshPath = "";
    std::string ShaderPath = "";
    std::string TexturePath = "";

    Vec3 Scale = Vec3();
    Vec3 Rotation = Vec3();
    Vec3 Position = Vec3();

    Vec3 SpecularColor = Vec3();
    float Shininess = 0.f;

    Vec3 EmissiveColor = Vec3();
    float EmissivePower = 0.f;

    ShapeComponentData ShapeData = ShapeComponentData();
};

struct PrefabCharacterData
{
    std::string Name = "";
    UINT m_eActorType = 0;

    std::string RootMeshPath = "";
    std::string MeshPath = "";
    std::string ShaderPath = "";
    std::string TexturePath = "";

    int ScriptType = 0;
    std::string ScriptName = "";

    int AnimIndex = 0;
    float AnimSpeed = 0.f;

    TransformData transform = TransformData();

    // Component
    CameraComponentData camera = CameraComponentData();
    ShapeComponentData shape = ShapeComponentData();

    struct ChildMeshData
    {
        std::string MeshPath = "";
        int TargetBoneIndex = 0;
    };
    std::vector<ChildMeshData> ChildMeshes;

};

struct PrefabUIData
{
    std::string Name = "";
    bool isTextUI = false;

    TransformData transform = TransformData();

    float color[4] = { 0.f, 0.f, 0.f, 0.f };
	float SliceUV[4] = { 0.f, 0.f, 0.f, 0.f };

    std::string IdleTexturePath = "";
    std::string HoverTexturePath = "";
    std::string ActiveTexturePath = "";
    std::string SelectedTexturePath = "";
    std::string ShaderPath = "";

    std::string FontPath = "";
    float FontSize = 20.f;
    std::string Text = "Hello World";

    int ScriptType = -1;
};

enum class EParticleType
{
    Default = 0,
    Fire,
};

struct PrefabParticleData
{
    std::string Name = "";

    std::string ShaderPath = "";
    std::string TexturePath = "";

    Vec3 Scale = Vec3();
    Vec3 Rotation = Vec3();
    Vec3 Translation = Vec3();

    int Divisions = 4;     // NxN
    int Row = 0;
    int Col = 0;

    float BillboardSizeX = 100.0f;
    float BillboardSizeY = 100.0f;

    float Duration = 1.0f;
    bool bLoop = true;
    bool bAutoDestroy = false;

    EParticleType Type = EParticleType::Default;
};

struct PrefabParticleGroupData
{
    std::string GroupName = "";
    std::vector<PrefabParticleData> Particles;
};

struct ModifiedCellData
{
    int Row = 0;
    int Col = 0;
    float Height = 0.f;
};

struct PrefabMapData
{
    int Cols = 0;
    int Rows = 0;
    float CellSize = 0.0f;

    Vec3 Position = Vec3();
    Vec3 Rotation = Vec3();
    Vec3 Scale = Vec3();

    int SelectedRow = -1;
    int SelectedCol = -1;
    float TargetHeight = 0.0f;

    std::string TexturePath = "";
    std::string ShaderPath = "";
};