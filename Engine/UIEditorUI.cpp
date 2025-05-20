#include "pch.h"
#include "UIEditorUI.h"
#include "PrefabLoader.h"
#include "AUIActor.h"

void UIEditorUI::DrawUI()
{
	// ───────────────────────────── Create ─────────────────────────────
	if (ImGui::Button("Create New UI"))
	{
		m_pUIActor = make_shared<AUIActor>();

		ResetData();

		// 초기값 세팅
		m_OnCreate(
			m_pUIActor,
			m_szTexturePath,
			m_szShaderPath,
			m_UIData
		);
	}

	if (m_pUIActor == nullptr)
		return;

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	// ───────────────────────────── Transform ─────────────────────────────
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
	DrawVec3("Position", m_UIData.Position);
	DrawVec3("Rotation", m_UIData.Rotation);
	DrawVec3("Scale", m_UIData.Scale);

	// ───────────────────────────── Actor Update ─────────────────────────────
	UpdateUIActor();

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	// ───────────────────────────── Asset Input ─────────────────────────────
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset Paths");
	ImGui::InputText("Texture", m_szTexturePath, IM_ARRAYSIZE(m_szTexturePath));
	ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

	ImGui::Spacing();
	// ───────────────────────────── Prefab Save/Load ─────────────────────────────
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

	static char prefabName[64] = "MyUI";
	ImGui::InputText("Prefab Name", prefabName, IM_ARRAYSIZE(prefabName));

	if (ImGui::Button("Save as Prefab", ImVec2(-1, 0)))
	{
		PrefabObjectData data;
		data.Name = prefabName;
		data.ShaderPath = m_szShaderPath;
		data.TexturePath = m_szTexturePath;
		data.Scale = Vec3(m_UIData.Scale);
		data.Rotation = Vec3(m_UIData.Rotation);
		data.Translation = Vec3(m_UIData.Position);

		PrefabLoader().SaveObject(data, "../Resources/Prefab/" + data.Name + ".ui.json");
	}

	if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
	{
		PrefabObjectData data;
		std::string path = "../Resources/Prefab/" + std::string(prefabName) + ".ui.json";
		if (PREFAB->LoadObject(path, data))
		{
			strcpy_s(m_szShaderPath, data.ShaderPath.c_str());
			strcpy_s(m_szTexturePath, data.TexturePath.c_str());

			
		}
	}

	ImGui::Separator(); ImGui::Spacing();
}

void UIEditorUI::DrawVec3(const char* label, float* values)
{
	ImGui::PushID(label);
	ImGui::Text("%s", label);
	ImGui::Indent(10.0f);

	static const char* axes[] = { "X", "Y", "Z" };
	const float buttonSize = 20.0f;
	const float inputWidth = 60.0f;
	const float sliderWidth = 150.0f;
	const float spacing = 4.0f;

	for (int i = 0; i < 3; ++i)
	{
		ImGui::PushID(i);

		ImGui::Text("%s", axes[i]); ImGui::SameLine();

		if (ImGui::Button("-", ImVec2(buttonSize, 0)))
			values[i] -= 0.1f;
		ImGui::SameLine(0, spacing);

		ImGui::PushItemWidth(inputWidth);
		ImGui::InputFloat("##Value", &values[i], 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine(0, spacing);

		if (ImGui::Button("+", ImVec2(buttonSize, 0)))
			values[i] += 0.1f;

		ImGui::PopID();
	}

	ImGui::Unindent(10.0f);
	ImGui::PopID();
}

void UIEditorUI::UpdateUIActor()
{
	if(m_pUIActor==nullptr)
		return;

	m_pUIActor->SetPosition(Vec3(m_UIData.Position));
	m_pUIActor->SetRotation(Vec3(m_UIData.Rotation));
	m_pUIActor->SetScale(Vec3(m_UIData.Scale));
}

void UIEditorUI::ResetData()
{
	for (int i = 0; i < 3; i++) m_UIData.Position[i] = 0.0f;
	for (int i = 0; i < 3; i++) m_UIData.Rotation[i] = 0.0f;
	for (int i = 0; i < 3; i++) m_UIData.Scale[i] = 100.0f;
	strcpy_s(m_szTexturePath, "../Resources/Texture/white.png");
	strcpy_s(m_szShaderPath, "../Resources/Shader/Default.hlsl");
}
