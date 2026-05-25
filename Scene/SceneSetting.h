#pragma once
#include "SceneSuper.h"
#include <DxLib.h>

class FileManager;

class SceneSetting : public SceneSuper
{
public:
	explicit SceneSetting(FileManager&);
	void Update() override;
	void Draw() override;
};