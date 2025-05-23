#pragma once

class ActorListUI
{
public:
	void DrawUI();
	void Toggle();
	void ViewTransformData(const std::shared_ptr<class AActor>& _actor);
	void ViewActorInformation(const std::shared_ptr<class AActor>& _actor);
	void ViewMeshComponentData(const std::shared_ptr<class AActor>& _actor);
	void ViewShapeComponentData(const std::shared_ptr<class AActor>& _actor);
	
	void ViewMapData(const std::shared_ptr<class ATerrainTileActor>& _tileactor);
	void ViewCharacterData(const std::shared_ptr<class AActor>& _actor);

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

