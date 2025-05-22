#include "pch.h"
#include "AAsset.h"
#include "APawn.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "SkeletalMeshData.h"
#include "UMaterial.h"
#include "fstream"

void AAsset::Export(TFbxResource _result, string filepath)
{
	FILE* pFile;
	wstring fileName = SplitName(to_mw(filepath.c_str()));
	string path = "../Resources/Asset/";
	string ext = ".asset";
	path = path + to_wm(fileName.c_str()) + ext;
	errno_t err = fopen_s(&pFile, path.c_str(), "wb");
	if (err != 0) { assert(false); }

	// HEADER
	fwrite(&_result.m_iBoneCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iMeshCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iAnimTrackCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iTexPathCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iNodeCount, sizeof(UINT), 1, pFile);

	// TEX PATH
	{
		for (auto& iter : _result.m_mTexPathList)
		{
			UINT texPathNameSize = static_cast<UINT>(iter.second.size());
			fwrite(&texPathNameSize, sizeof(UINT), 1, pFile);
			fwrite(&iter.first, sizeof(int), 1, pFile);
			fwrite(iter.second.data(), sizeof(wchar_t), texPathNameSize, pFile);
		}
	}


	// ANIM DATA
	{
		for (int iAnimTrack = 0; iAnimTrack < _result.m_iAnimTrackCount; iAnimTrack++)
		{
			// NODE NAME
			UINT nameSize = static_cast<UINT>(_result.m_vAnimTrackList[iAnimTrack].m_szName.size());
			fwrite(&nameSize, sizeof(UINT), 1, pFile);
			fwrite(_result.m_vAnimTrackList[iAnimTrack].m_szName.data(), sizeof(wchar_t), nameSize, pFile);

			//
			fwrite(&_result.m_vAnimTrackList[iAnimTrack].m_iStartFrame, sizeof(UINT), 1, pFile);
			fwrite(&_result.m_vAnimTrackList[iAnimTrack].m_iEndFrame, sizeof(UINT), 1, pFile);

			UINT frameCount = _result.m_vAnimTrackList[iAnimTrack].m_vAnim[0].size();
			fwrite(&frameCount, sizeof(UINT), 1, pFile);
			for (int iBone = 0; iBone < _result.m_iNodeCount; iBone++)
			{
				fwrite(&_result.m_vAnimTrackList[iAnimTrack].m_vAnim[iBone].at(0), sizeof(Matrix), frameCount, pFile);
			}
		}
	}

	// BONE
	for (auto& data : _result.m_mSkeletonList)
	{
		// BONE NAME
		UINT nameSize = static_cast<UINT>(data.second.m_szName.size());
		fwrite(&nameSize, sizeof(UINT), 1, pFile);
		fwrite(data.second.m_szName.data(), sizeof(wchar_t), nameSize, pFile);

		// BONE DATA
		fwrite(&data.second.m_iIndex, sizeof(UINT), 1, pFile);
		UINT parentNameSize = static_cast<UINT>(data.second.m_szParentName.size());
		fwrite(&parentNameSize, sizeof(UINT), 1, pFile);
		fwrite(data.second.m_szParentName.data(), sizeof(wchar_t), parentNameSize, pFile);
	}

	// INVERSE MAT BONE
	for (int iMat = 0; iMat < _result.m_iMeshCount; iMat++)
	{
		if (_result.m_iBoneCount <= 0) continue; // ????..??
		fwrite(&_result.m_vInverseBindPose[iMat].at(0), sizeof(Matrix), _result.m_iNodeCount, pFile);
	}

	// MESH NODES
	for (int iMesh = 0; iMesh < _result.m_iMeshCount; iMesh++)
	{
		auto& child = _result.m_vMeshList[iMesh];
		// HEADER FOR NODE
		UINT nameSize = child.m_szName.size();
		fwrite(&nameSize, sizeof(UINT), 1, pFile);
		fwrite(child.m_szName.data(), sizeof(wchar_t), nameSize, pFile);

		fwrite(&child.m_bSkeleton, sizeof(bool), 1, pFile);
		UINT vertexSize = child.m_vVertexList.size();
		fwrite(&vertexSize, sizeof(UINT), 1, pFile);
		UINT iwSize = child.m_vIwList.size();
		fwrite(&iwSize, sizeof(UINT), 1, pFile);
		UINT indexSize = child.m_vIndexList.size();
		fwrite(&indexSize, sizeof(UINT), 1, pFile);
		fwrite(&child.m_iMaterialIndex, sizeof(UINT), 1, pFile);

		// VERTEX
		fwrite(&child.m_vVertexList.at(0), sizeof(PNCT_VERTEX), vertexSize, pFile);
		if (iwSize > 0)
		{
			fwrite(&child.m_vIwList.at(0), sizeof(IW_VERTEX), iwSize, pFile);
		}
		if (indexSize > 0)
		{
			fwrite(&child.m_vIndexList.at(0), sizeof(DWORD), indexSize, pFile);
		}
	}

	fclose(pFile);

}

TFbxResource AAsset::Load(const char* fileName)
{
	string file = fileName;
	Profiler p("Asset::Load" + file);
	FILE* pFile;

	errno_t err = fopen_s(&pFile, fileName, "rb");
	if (err != 0) { assert(false); }
	TFbxResource result;
	result.name = SplitName(to_mw(fileName));
	result.m_ResPathName = to_mw(fileName);
	// HEADER
	fread(&result.m_iBoneCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iMeshCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iAnimTrackCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iTexPathCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iNodeCount, sizeof(UINT), 1, pFile);

	//TEX PATH
	for (int iTex = 0; iTex < result.m_iTexPathCount; iTex++)
	{
		int texPathNameSize;
		fread(&texPathNameSize, sizeof(UINT), 1, pFile);
		int texPathIndex;
		wstring texPath;
		texPath.resize(texPathNameSize);
		fread(&texPathIndex, sizeof(int), 1, pFile);
		fread(&texPath[0], sizeof(wchar_t), texPathNameSize, pFile);

		result.m_mTexPathList.insert(make_pair(texPathIndex, texPath));
	}

	// ANIM DATA
	result.m_vAnimTrackList.resize(result.m_iAnimTrackCount);
	for (int iAnimTrack = 0; iAnimTrack < result.m_iAnimTrackCount; iAnimTrack++)
	{
		// NODE NAME
		int nameSize = 0;
		fread(&nameSize, sizeof(UINT), 1, pFile);
		result.m_vAnimTrackList[iAnimTrack].m_szName.resize(nameSize);
		fread(&result.m_vAnimTrackList[iAnimTrack].m_szName[0], sizeof(wchar_t), nameSize, pFile);

		fread(&result.m_vAnimTrackList[iAnimTrack].m_iStartFrame, sizeof(UINT), 1, pFile);
		fread(&result.m_vAnimTrackList[iAnimTrack].m_iEndFrame, sizeof(UINT), 1, pFile);


		UINT frameCount = 0;
		fread(&frameCount, sizeof(UINT), 1, pFile);

		result.m_vAnimTrackList[iAnimTrack].m_vAnim.resize(result.m_iNodeCount);
		for (int iBone = 0; iBone < result.m_iNodeCount; iBone++)
		{
			result.m_vAnimTrackList[iAnimTrack].m_vAnim[iBone].resize(frameCount);
			fread(&result.m_vAnimTrackList[iAnimTrack].m_vAnim[iBone].at(0), sizeof(Matrix), frameCount, pFile);
		}
	}

	// BONE
	for (int iBone = 0; iBone < result.m_iBoneCount; iBone++)
	{
		BoneNode bone;
		// BONE NAME
		UINT nameSize;
		fread(&nameSize, sizeof(UINT), 1, pFile);
		bone.m_szName.resize(nameSize);
		fread(&bone.m_szName[0], sizeof(wchar_t), nameSize, pFile);

		// BONE DATA
		fread(&bone.m_iIndex, sizeof(UINT), 1, pFile);

		UINT parentNameSize;
		fread(&parentNameSize, sizeof(UINT), 1, pFile);
		bone.m_szParentName.resize(parentNameSize);
		fread(&bone.m_szParentName[0], sizeof(wchar_t), parentNameSize, pFile);

		result.m_mSkeletonList.insert(make_pair(bone.m_iIndex, bone));
	}


	// INVERSE MAT BONE
	result.m_vInverseBindPose.resize(result.m_iMeshCount);
	for (int iMat = 0; iMat < result.m_iMeshCount; iMat++)
	{
		if(result.m_iBoneCount <= 0) { continue; }
		result.m_vInverseBindPose[iMat].resize(result.m_iNodeCount);
		fread(&result.m_vInverseBindPose[iMat].at(0), sizeof(Matrix), result.m_iNodeCount, pFile);
	}

	// MESH NODES
	result.m_vMeshList.resize(result.m_iMeshCount);
	for (int iMesh = 0; iMesh < result.m_iMeshCount; iMesh++)
	{
		auto& child = result.m_vMeshList[iMesh];
		// HEADER FOR NODE
		UINT nameSize;
		fread(&nameSize, sizeof(UINT), 1, pFile);
		child.m_szName.resize(nameSize);
		fread(&child.m_szName[0], sizeof(wchar_t), nameSize, pFile);
		
		fread(&child.m_bSkeleton, sizeof(bool), 1, pFile);
		UINT vertexSize = 0;
		fread(&vertexSize, sizeof(UINT), 1, pFile);
		UINT iwSize = 0;
		fread(&iwSize, sizeof(UINT), 1, pFile);
		UINT indexSize = 0;
		fread(&indexSize, sizeof(UINT), 1, pFile);
		fread(&child.m_iMaterialIndex, sizeof(UINT), 1, pFile);

		// VERTEX
		child.m_vVertexList.resize(vertexSize);
		fread(&child.m_vVertexList.at(0), sizeof(PNCT_VERTEX), vertexSize, pFile);
		if (iwSize > 0)
		{
			child.m_vIwList.resize(iwSize);
			fread(&child.m_vIwList.at(0), sizeof(IW_VERTEX), iwSize, pFile);
		}
		if (indexSize > 0)
		{
			child.m_vIndexList.resize(indexSize);
			fread(&child.m_vIndexList.at(0), sizeof(DWORD), indexSize, pFile);
		}
	}
	
	fclose(pFile);
	return result;
}

void AAsset::ExportMesh(shared_ptr<APawn> _actor, string filepath)
{
	FILE* pFile;
	string path = "../Resources/Asset/";
	string ext = ".mesh";
	path = path + filepath + ext;
	errno_t err = fopen_s(&pFile, path.c_str(), "wb");
	if (err != 0) { assert(false); }

	// ROOT MESH
	auto mesh = _actor->GetMeshComponent();
	bool isSkinned = (dynamic_pointer_cast<USkinnedMeshComponent>(mesh) != nullptr);
	if (isSkinned)
	{
		ExportSkinned(pFile, mesh);
	}
	else
	{
		ExportStatic(pFile, mesh);
	}

	int childCount = mesh->GetChildren().size();
	fwrite(&childCount, sizeof(int), 1, pFile);
	for (auto& child : mesh->GetChildren())
	{
		bool isSkinned = (dynamic_pointer_cast<USkinnedMeshComponent>(child) != nullptr);
		if (isSkinned)
		{
			ExportSkinned(pFile, child);
		}
		else
		{
			ExportStatic(pFile, child);
		}
	}

	fclose(pFile);
}

void AAsset::ExportSkinned(FILE* pFile, shared_ptr<UMeshComponent> _mesh)
{
	auto mesh = dynamic_pointer_cast<USkinnedMeshComponent>(_mesh);

	// BASE
	int type = mesh->GetMesh()->GetType();
	fwrite(&type, sizeof(int), 1, pFile);

	wstring resName = mesh->GetMesh()->GetName();
	ExportWstring(pFile, resName);

	// ANIM
	bool bHasBaseAnim = mesh->GetAnimInstance() ? true : false;
	fwrite(&bHasBaseAnim, sizeof(bool), 1, pFile);
	if (bHasBaseAnim)
	{
		auto animInstance = mesh->GetAnimInstance();
		wstring animName = animInstance->GetName();
		ExportWstring(pFile, animName);
		bool bInPlace = animInstance->m_bInPlace;
		int rootIndex = animInstance->GetRootIndex();
		fwrite(&bInPlace, sizeof(bool), 1, pFile);
		fwrite(&rootIndex, sizeof(int), 1, pFile);
	}
	
	// MATERIAL
	wstring texPath = mesh->GetMaterial()->GetTexture()->m_pFilePath;
	ExportWstring(pFile, texPath);

	wstring shaderPath = mesh->GetMaterial()->GetShaderPath();
	ExportWstring(pFile, shaderPath);

	// SRT
	Vec3 pos = mesh->GetLocalPosition();
	Vec3 rot = mesh->GetLocalRotation();
	Vec3 scale = mesh->GetLocalScale();
	fwrite(&pos, sizeof(Vec3), 1, pFile);
	fwrite(&rot, sizeof(Vec3), 1, pFile);
	fwrite(&scale, sizeof(Vec3), 1, pFile);
}
void AAsset::ExportStatic(FILE* pFile, shared_ptr<UMeshComponent> _mesh)
{
	auto mesh = dynamic_pointer_cast<UStaticMeshComponent>(_mesh);

	// BASE
	int type = mesh->GetMesh()->GetType();
	fwrite(&type, sizeof(int), 1, pFile);

	wstring resName = mesh->GetMesh()->GetName();
	ExportWstring(pFile, resName);

	// ANIM
	bool bHasBaseAnim = mesh->GetAnimInstance() ? true : false;
	fwrite(&bHasBaseAnim, sizeof(bool), 1, pFile);
	if (bHasBaseAnim)
	{
		auto animInstance = mesh->GetAnimInstance();
		wstring animName = animInstance->GetName();
		ExportWstring(pFile, animName);
		int targetBone = mesh->GetTargetBoneIndex();
		fwrite(&targetBone, sizeof(int), 1, pFile);
		Matrix matBone = mesh->GetMatBone();
		fwrite(&matBone, sizeof(Matrix), 1, pFile);
	}

	

	// MATERAIL
	wstring texPath = mesh->GetMaterial()->GetTexture()->m_pFilePath;
	ExportWstring(pFile, texPath);
	wstring shaderPath = mesh->GetMaterial()->GetShaderPath();
	ExportWstring(pFile, shaderPath);

	// SRT
	Vec3 pos = mesh->GetLocalPosition();
	Vec3 rot = mesh->GetLocalRotation();
	Vec3 scale = mesh->GetLocalScale();
	fwrite(&pos, sizeof(Vec3), 1, pFile);
	fwrite(&rot, sizeof(Vec3), 1, pFile);
	fwrite(&scale, sizeof(Vec3), 1, pFile);
}

void AAsset::ExportJsonMesh(shared_ptr<APawn> _actor, string fileName)
{
	string path = "../Resources/Asset/";
	fileName += ".mesh.json";
	fileName = path + fileName;

	std::ofstream file(fileName);
	if (!file.is_open()) { assert(false); }

	nlohmann::ordered_json j;
	int i = 0;

	// ROOT MESH
	auto mesh = _actor->GetMeshComponent();
	bool isSkinned = (dynamic_pointer_cast<USkinnedMeshComponent>(mesh) != nullptr);
	if (isSkinned)
	{
		ExportSkinned(i++, j, mesh);
	}
	else
	{
		ExportStatic(i++, j, mesh);
	}

	int childCount = mesh->GetChildren().size();
	j["childCount"] = childCount;
	for (auto& child : mesh->GetChildren())
	{
		bool isSkinned = (dynamic_pointer_cast<USkinnedMeshComponent>(child) != nullptr);
		if (isSkinned)
		{
			ExportSkinned(i++, j, child);
		}
		else
		{
			ExportStatic(i++, j, child);
		}
	}

	file << j.dump(4); // 예쁘게 출력
}

void AAsset::ExportSkinned(int num, nlohmann::ordered_json& j, shared_ptr<UMeshComponent> _mesh)
{
	auto mesh = dynamic_pointer_cast<USkinnedMeshComponent>(_mesh);

	// BASE
	j["Mesh" + to_string(num)];
	int type = mesh->GetMesh()->GetType();
	j["MeshType" + to_string(num)] = type;
	string resName = to_wm(mesh->GetMesh()->GetName());
	j["MeshResName" + to_string(num)] = resName;

	// ANIM
	bool bHasBaseAnim = mesh->GetAnimInstance() ? true : false;
	j["HasBaseAnim" + to_string(num)] = bHasBaseAnim;
	
	if (bHasBaseAnim)
	{
		auto animInstance = mesh->GetAnimInstance();
		string animName = to_wm(animInstance->GetName());
		j["animName" + to_string(num)] = animName;
		bool bInPlace = animInstance->m_bInPlace;
		int rootIndex = animInstance->GetRootIndex();
		j["bInPlace" + to_string(num)] = bInPlace;
		j["rootIndex" + to_string(num)] = rootIndex;
	}

	// MATERIAL
	string texPath = to_wm(mesh->GetMaterial()->GetTexture()->m_pFilePath);
	j["texPath" + to_string(num)] = texPath;

	string shaderPath = to_wm(mesh->GetMaterial()->GetShaderPath());
	j["shaderPath" + to_string(num)] = shaderPath;

	// SRT
	Vec3 pos = mesh->GetLocalPosition();
	Vec3 rot = mesh->GetLocalRotation();
	Vec3 scale = mesh->GetLocalScale();

	j["Scale" + to_string(num)] = { scale.x, scale.y, scale.z };
	j["Rotation" + to_string(num)] = { rot.x, rot.y, rot.z };
	j["Translation" + to_string(num)] = { pos.x, pos.y, pos.z };
}

void AAsset::ExportStatic(int num, nlohmann::ordered_json& j, shared_ptr<UMeshComponent> _mesh)
{
	auto mesh = dynamic_pointer_cast<UStaticMeshComponent>(_mesh);

	// BASE
	int type = mesh->GetMesh()->GetType();
	j["MeshType" + to_string(num)] = type;

	string resName = to_wm(mesh->GetMesh()->GetName());
	j["MeshResName" + to_string(num)] = resName;

	// ANIM
	bool bHasBaseAnim = mesh->GetAnimInstance() ? true : false;
	j["HasBaseAnim" + to_string(num)] = bHasBaseAnim;

	if (bHasBaseAnim)
	{
		auto animInstance = mesh->GetAnimInstance();
		string animName = to_wm(animInstance->GetName());
		j["animName" + to_string(num)] = animName;
		int targetBone = mesh->GetTargetBoneIndex();
		j["targetBone" + to_string(num)] = targetBone;
		Matrix matBone = mesh->GetMatBone();
		j["matBone" + to_string(num)] = MatrixToJson(matBone);
	}

	// MATERAIL
	string texPath = to_wm(mesh->GetMaterial()->GetTexture()->m_pFilePath);
	j["texPath" + to_string(num)] = texPath;
	
	string shaderPath = to_wm(mesh->GetMaterial()->GetShaderPath());
	j["shaderPath" + to_string(num)] = shaderPath;

	// SRT
	Vec3 pos = mesh->GetLocalPosition();
	Vec3 rot = mesh->GetLocalRotation();
	Vec3 scale = mesh->GetLocalScale();

	j["Scale" + to_string(num)] = { scale.x, scale.y, scale.z };
	j["Rotation" + to_string(num)] = { rot.x, rot.y, rot.z };
	j["Translation" + to_string(num)] = { pos.x, pos.y, pos.z };

}

MeshComponentData AAsset::LoadJsonMesh(const char* filepath)
{
	//Profiler p("AAsset::LoadJsonMesh");
	std::ifstream file(filepath);
	if (!file.is_open()) { assert(false); }

	nlohmann::ordered_json j;
	file >> j;

	int num = 0;
	MeshComponentData root = LoadOneJsonMesh(num++, j);

	int childCount = j["childCount"];

	for (int i = 0; i < childCount; i++)
	{
		MeshComponentData child = LoadOneJsonMesh(num++, j);
		root.m_vChild.emplace_back(child);
	}

	return root;
}

MeshComponentData AAsset::LoadOneJsonMesh(int num, const nlohmann::ordered_json& j)
{
	MeshComponentData ret;

	// BASE
	ret.m_type = j["MeshType" + to_string(num)];
	ret.m_szRes = to_mw(j["MeshResName" + to_string(num)]);
	bool bHasBaseAnim = (j["HasBaseAnim" + to_string(num)]);

	if (bHasBaseAnim)
	{
		if (ret.m_type == (int)MeshType::M_SKINNED)
		{
			ret.m_szAnim = to_mw(j["animName" + to_string(num)]);
			ret.m_bInPlace = j["bInPlace" + to_string(num)];
			ret.m_rootIndex = j["rootIndex" + to_string(num)];
		}
		else
		{
			ret.m_szAnim = to_mw(j["animName" + to_string(num)]);
			ret.m_targetBone = j["targetBone" + to_string(num)];
			ret.m_matBone = LoadMatrixFromJson(j["matBone" + to_string(num)]);
		}
	}

	// MATERIAL
	ret.m_szTex = to_mw(j["texPath" + to_string(num)]);
	ret.m_szShader = to_mw(j["shaderPath" + to_string(num)]);

	// SRT
	auto s = j["Scale" + to_string(num)];
	auto r = j["Rotation" + to_string(num)];
	auto t = j["Translation" + to_string(num)];
	ret.m_scale = Vec3(s[0], s[1], s[2]);
	ret.m_rot = Vec3(r[0], r[1], r[2]);
	ret.m_pos = Vec3(t[0], t[1], t[2]);

	return ret;
}

MeshComponentData AAsset::LoadMesh(const char* filepath)
{
	FILE* pFile;
	errno_t err = fopen_s(&pFile, filepath, "rb");
	if (err != 0) { assert(false); }

	MeshComponentData root = LoadOneMesh(pFile);

	int childCount;
	fread(&childCount, sizeof(int), 1, pFile);
	for (int i = 0; i < childCount; i++)
	{
		MeshComponentData child = LoadOneMesh(pFile);
		root.m_vChild.emplace_back(child);
	}
	
	fclose(pFile);
	return root;

}

MeshComponentData AAsset::LoadOneMesh(FILE* pFile)
{
	MeshComponentData ret;
	int meshType;
	fread(&meshType, sizeof(int), 1, pFile);
	ret.m_type = meshType;
	ret.m_szRes = LoadWstring(pFile);

	if (meshType == (int)MeshType::M_SKINNED)
	{
		bool bHasBaseAnim;
		fread(&bHasBaseAnim, sizeof(bool), 1, pFile);
		if (bHasBaseAnim)
		{
			ret.m_szAnim = LoadWstring(pFile);
			fread(&ret.m_bInPlace, sizeof(bool), 1, pFile);
			fread(&ret.m_rootIndex, sizeof(int), 1, pFile);
		}
		ret.m_szTex = LoadWstring(pFile);
		ret.m_szShader = LoadWstring(pFile);
		fread(&ret.m_pos, sizeof(Vec3), 1, pFile);
		fread(&ret.m_rot, sizeof(Vec3), 1, pFile);
		fread(&ret.m_scale, sizeof(Vec3), 1, pFile);
	}
	else
	{
		bool bHasBaseAnim;
		fread(&bHasBaseAnim, sizeof(bool), 1, pFile);
		if (bHasBaseAnim)
		{
			ret.m_szAnim = LoadWstring(pFile);
			fread(&ret.m_targetBone, sizeof(int), 1, pFile);
			fread(&ret.m_matBone, sizeof(Matrix), 1, pFile);
		}
		ret.m_szTex = LoadWstring(pFile);
		ret.m_szShader = LoadWstring(pFile);
		fread(&ret.m_pos, sizeof(Vec3), 1, pFile);
		fread(&ret.m_rot, sizeof(Vec3), 1, pFile);
		fread(&ret.m_scale, sizeof(Vec3), 1, pFile);
	}
	return ret;
}

void AAsset::ExportWstring(FILE* pFile, wstring _str)
{
	UINT strSize = static_cast<UINT>(_str.size());
	fwrite(&strSize, sizeof(UINT), 1, pFile);
	fwrite(_str.data(), sizeof(wchar_t), strSize, pFile);
}

wstring AAsset::LoadWstring(FILE* pFile)
{
	UINT strSize;
	fread(&strSize, sizeof(UINT), 1, pFile);
	wstring str;
	str.resize(strSize);
	fread(&str[0], sizeof(wchar_t), strSize, pFile);

	return str;
}

json AAsset::MatrixToJson(const Matrix& mat)
{
	json j = json::array();
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			j.push_back(mat(row, col)); // 열 우선
		}
	}
	return j;
}

Matrix AAsset::LoadMatrixFromJson(const nlohmann::ordered_json& j)
{
	Matrix mat;

	if (!j.is_array() || j.size() != 16) {
		throw std::runtime_error("Invalid matrix JSON format.");
	}

	// column-major: fill mat(row, col) using column-major index
	int index = 0;
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			mat(row, col) = j.at(index++).get<float>();
		}
	}

	return mat;
}


