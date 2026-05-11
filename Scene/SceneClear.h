#pragma once
#include "SceneSuper.h"
#include <DxLib.h>

class FileManager;

class SceneClear : public SceneSuper
{
public:
	// クリアシーン生成
	explicit SceneClear(FileManager&);
	// 入力更新
	void Update() override;
	// 画面描画
	void Draw() override;
};
