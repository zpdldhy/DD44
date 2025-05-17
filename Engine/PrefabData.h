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
    std::string MeshPath;
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

struct PrefabMapData
{
    int Cols;
    int Rows;
    float CellSize;

    Vec3 Position;
    Vec3 Rotation;
    Vec3 Scale;

    std::string TexturePath;
    std::string ShaderPath;
};