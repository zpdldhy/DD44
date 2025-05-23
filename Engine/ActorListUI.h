#pragma once

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
	void SaveObjectPrefab(const std::shared_ptr<class AActor>& _actor);

	UINT GetLastActorID();
	UINT GetSelectedActorID();
	UINT GetSelectedGizmoAxis();

	void SetSelectedActorID(UINT id);
	void SetSelectedGizmoAxis(UINT id);

private:
	bool m_bVisible = false;
	float m_fSlideOffset = 0.0f;

	Vec3 m_vPosition;
	Vec3 m_vRotation;
	Vec3 m_vScale;
	std::wstring m_szNewName;

	int m_iLastActorID = -1;
	int m_iSelectedActorID = -1;
	int m_iSelectedGizmoAxis = -1;
};

