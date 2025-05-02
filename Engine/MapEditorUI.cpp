#include "pch.h"
#include "MapEditorUI.h"

void MapEditorUI::Update()
{
	ImGui::Begin("Map Editor");

	// Section: Terrain Configuration
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Terrain Configuration");
	ImGui::InputInt("Num Cols", &numCols);
	ImGui::InputInt("Num Rows", &numRows);
	ImGui::InputFloat("Cell Size", &cellSize);
	ImGui::Separator(); ImGui::Spacing();

	// Section: Transform (SRT)
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Transform");
	ImGui::InputFloat3("Position", position);
	ImGui::InputFloat3("Rotation", rotation);
	ImGui::InputFloat3("Scale", scale);
	ImGui::Separator(); ImGui::Spacing();

	// Section: Appearance
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Appearance");
	ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));
	ImGui::InputText("Shader Path", shaderPath, IM_ARRAYSIZE(shaderPath));
	ImGui::Spacing(); ImGui::Spacing();

	// Action Button
	if (ImGui::Button("Create Terrain Tile", ImVec2(-1, 0)))  // -1 = full width
	{
		auto tile = make_shared<ATerrainTileActor>();
		tile->m_iNumCols = numCols;
		tile->m_iNumRows = numRows;
		tile->m_fCellSize = cellSize;

		tile->CreateTerrain(
			std::wstring(texturePath, texturePath + strlen(texturePath)),
			std::wstring(shaderPath, shaderPath + strlen(shaderPath))
		);

		tile->SetPosition(Vec3(position[0], position[1], position[2]));
		tile->SetRotation(Vec3(rotation[0], rotation[1], rotation[2]));
		tile->SetScale(Vec3(scale[0], scale[1], scale[2]));

		tile->Init();
		m_vTiles.push_back(tile);
	}

	// Debug Options
	ImGui::Separator(); ImGui::Spacing();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Debug Options");
	ImGui::Checkbox("Wireframe Mode", &m_bEditorWireframe);

	ImGui::End();
}
