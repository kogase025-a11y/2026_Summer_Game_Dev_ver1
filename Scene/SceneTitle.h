#pragma once
#include "SceneSuper.h"
#include "../Manager/FileManager.h"
#include "../Resource/ImageFile.h"
#include <memory>
#include <DxLib.h>

class FileManager;
class ImageFile;

class SceneTitle : public SceneSuper
{
public:
	// タイトルシーン生成
	explicit SceneTitle(FileManager& fileMng);

	void Update() override;
	// 画面描画
	void Draw() override;
private:
	std::shared_ptr<ImageFile> imageFile_;
	

private:
	std::shared_ptr<ImageFile> titleBg_;

};
