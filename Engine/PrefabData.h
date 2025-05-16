#pragma once

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
};

struct PrefabCharacterData
{
    std::string Name;

    std::string RootMeshPath;
    std::string ShaderPath;
    std::string TexturePath;

    int ScriptType;

    int AnimIndex;
    float AnimSpeed;

    Vec3 Scale;
    Vec3 Rotation;
    Vec3 Translation;

    struct ChildMeshData
    {
        std::string MeshPath;
        int TargetBoneIndex;
    };
    std::vector<ChildMeshData> ChildMeshes;
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