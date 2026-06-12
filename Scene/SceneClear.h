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

	float GetScoreY()const;
	float GetScoreXStart()const;
	float GetScoreXEnd()const;
	float GetScoreYTop()const;
	float GetScoreYAtX(float x) const;

private:
	float scoreY_;
	float scoreXStart_;
	float scoreXEnd_;
	float scoreYTop_;

	
	std::shared_ptr<ImageFile> clearBg_;
	
};
