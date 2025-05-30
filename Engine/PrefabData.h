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
    std::string Name;
    UINT m_eActorType;

    std::string MeshPath;
    std::string ShaderPath;
    std::string TexturePath;

    Vec3 Scale;
    Vec3 Rotation;
    Vec3 Position;

    Vec3 SpecularColor;
    float Shininess;

    Vec3 EmissiveColor;
    float EmissivePower;

    ShapeComponentData ShapeData;
};

struct PrefabCharacterData
{
    std::string Name;
    UINT m_eActorType;

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
    bool isTextUI = false;

    TransformData transform;

    float color[4] = { 0.f, 0.f, 0.f, 0.f };
	float SliceUV[4] = { 0.f, 0.f, 0.f, 0.f };

    std::string IdleTexturePath;
    std::string HoverTexturePath;
    std::string ActiveTexturePath;
    std::string SelectedTexturePath;
    std::string ShaderPath;

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

    EParticleType Type = EParticleType::Default;

    //Fire 타입 전용 파라미터
    Vec3 Amplitude = { 0.0f, 0.0f, 0.0f };
    Vec3 RandomFreq = { 0.0f, 0.0f, 0.0f };
    Vec3 RandomOffset = { 0.0f, 0.0f, 0.0f };
    float TimeOffset = 0.0f;
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