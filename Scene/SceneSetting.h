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

private:
	int cursorIndex_ = 0; // 0: BGM, 1: SE, 2: Back
	int bgmVolume_ = 50;
	int seVolume_ = 50;
	int prevPadState_ = 0;
};
