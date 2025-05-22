#pragma once
#include "Singleton.h"
#include "FbxLoader.h"
#include "AssimpLoader.h"
#include "SkeletalMeshData.h"

class APawn;
class UMeshComponent;
class UMeshResources;
class UAnimInstance;
class UMaterial;

class ActorLoader : public Singleton<ActorLoader>
{
public:
	ActorLoader(){}
	~ActorLoader() 
	{
		m_mMeshMap.clear();
	}
public:
	FbxLoader m_FbxImporter;
	AssimpLoader m_ObjImporter;

	vector<TFbxResource> m_vFbxList;
	map<wstring, shared_ptr<UMeshResources>> m_mMeshMap;

public:
	//Fbx Obj 파싱 
	void ConvertFbxToAsset(string _path = "../Resources/Fbx/DeathDoor/*.fbx");
	void ConvertObjToAsset(string _path = "../Resources/Obj/*.obj");
	void LoadAsFbxResource(string path);

	/**
	* @brief MeshResources MeshAsset의 사용
	*
	* 1. 시작 시 Asset 폴더의 .asset 파일 일괄 로딩 ( LoadAllAsset)
	* 2. MeshLoader로 MeshMap / AnimMap 전달 ( LoadMeshMap, LoadAnimMap )
	* @return /void ActorLoader 내부에서 정보 가지고 있음
	*/
	void LoadAllAsset();
	map<wstring, shared_ptr<UMeshResources>> LoadMeshMap();
	map < wstring, shared_ptr<UAnimInstance>> LoadAnimMap();

	////////////////////// WILL BE DEPRECATED ////////////////////////////////
	void LoadOne(string _path);
	/**
	* @brief MeshResources 반환. 
	* 
	* MeshLoader에서 역할 수행하도록 변경
	*
	*/
	vector<shared_ptr<UMeshResources>> LoadMeshResources();
	shared_ptr<UMeshResources> LoadOneRes(string _path);
	vector<MeshComponentData> LoadMeshData();
	vector<shared_ptr<UMeshComponent>> LoadMesh();
	vector<wstring> LoadTexPath();
	vector<shared_ptr<UAnimInstance>> LoadAnim();
	shared_ptr<APawn> LoadOneActor(string _path);
	vector<shared_ptr<APawn>> LoadAllActor();
	//[[deprecated("Use NewFunction() instead.")]]
	shared_ptr<APawn> LoadRedeemer(string _path);
	vector<MeshComponentData> m_vMeshDataList;
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
	vector<shared_ptr<UMeshResources>> m_vMeshResList;
	map<wstring, shared_ptr<UAnimInstance>> m_vAnimInstanceMap;
	map<wstring, shared_ptr<UMeshResources>> m_mResPathMap;
	vector<shared_ptr<UAnimInstance>> m_vAnimInstanceList;
	
};

