#pragma once

enum class GizmoMode
{
	Translate = 0,
	Rotate,
	Scale,
	None
};

class ActorListUI
{
public:
	void DrawUI();
	void Toggle();

	// 공통 데이터 정보
	void ViewTransformData(const std::shared_ptr<class AActor>& _actor);
	void ViewActorInformation(const std::shared_ptr<class AActor>& _actor);
	void ViewMeshComponentData(const std::shared_ptr<class AActor>& _actor);
	void ViewShapeComponentData(const std::shared_ptr<class AActor>& _actor);

	// 타입별 데이터 정보
	void ViewMapData(const std::shared_ptr<class ATerrainTileActor>& _tileactor);
	void ViewCharacterData(const std::shared_ptr<class AActor>& _actor);

	// 데이터 수정
	void EditActorTransform(std::shared_ptr<class AActor>& _actor);
	void EditActorName(std::shared_ptr<class AActor>& _actor);
	void EditShapeTransformData(std::shared_ptr<AActor>& _actor);

	// 타입별 저장
	void SaveMapPrefab(const std::shared_ptr<class ATerrainTileActor>& _tileactor);
	void SaveCharacterPrefab(const std::shared_ptr<class AActor>& _actor);
	void SaveObjectPrefab(const std::shared_ptr<class AActor>& _actor, const std::string& path);
	void SaveObjectsPrefab(const std::vector<std::shared_ptr<AActor>>& _actors, const std::string& path);
	void SaveAllObjectsInScene(const std::string& fileName);

	UINT GetLastActorID();
	UINT GetSelectedActorID();

	std::vector<UINT>& GetMultiSelectedIDs();
	const std::vector<UINT>& GetLastMultiSelectedIDs() const;

	void SetMultiSelectMode(bool _mode);
	bool IsMultiSelectMode() const;
	void RefreshSelectionVisuals();

	UINT GetSelectedGizmoAxis();
	GizmoMode GetMode() { return m_eGizmoMode; }

	void SetSelectedActorID(UINT id);
	void SetSelectedGizmoAxis(UINT id);
	void SetGizmoMode(GizmoMode _mode) { m_eGizmoMode = _mode; }

private:
	bool m_bVisible = true;
	float m_fSlideOffset = 0.0f;

	Vec3 m_vPosition;
	Vec3 m_vRotation;
	Vec3 m_vScale;

	std::wstring m_szNewName;
	char m_szSaveName[256] = "MyPrefab";
	char m_szSceneName[128] = "MyScene";

	int m_iLastActorID = -1;
	int m_iSelectedActorID = -1;
	int m_iSelectedGizmoAxis = -1;

	std::vector<UINT> m_vMultiSelectedIDs;
	std::vector<UINT> m_vLastMultiSelectedIDs;
	bool m_bMultiSelectMode = false;

	GizmoMode m_eGizmoMode = GizmoMode::None;
};

