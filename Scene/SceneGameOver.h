#pragma once
#include "SceneSuper.h"
#include <DxLib.h>
#include <memory>

class FileManager;
class ImageFile;

class SceneGameOver :public SceneSuper
{
	public:
	SceneGameOver(FileManager& fileMng);
	void Update()override;
	void Draw()override;

private:
	std::shared_ptr<ImageFile> gameOverBg_;

};

