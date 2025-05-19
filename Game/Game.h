#pragma once
#include "IExecute.h"

class Game : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

protected:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();

	void SetupEditorCallbacks();
	void SetupCharacterEditorCallback();
	void SetupMapEditorCallback();
	void SetupObjectEditorCallback();

	void LoadAllPrefabs(const std::string& extension);

};

