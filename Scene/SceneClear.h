#pragma once
#include "SceneSuper.h"
#include <DxLib.h>
#include <memory>

class FileManager;
class ImageFile;

class SceneClear : public SceneSuper
{
public:
	// クリアシーン生成
	explicit SceneClear(FileManager&);
	// 入力更新
	void Update() override;
	// 画面描画
	void Draw() override;

private:
	std::shared_ptr<ImageFile> clearBg_;
};
