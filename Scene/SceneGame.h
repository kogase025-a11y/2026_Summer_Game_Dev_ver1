#pragma once
#include "SceneSuper.h"
#include "../Input/InputManager.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include <DxLib.h>
#include <memory>

class FileManager;
class SceneManager;
class ImageFile;

class SceneGame : public SceneSuper
{
public:
	// ゲームシーン生成
	SceneGame(FileManager& fileMng, SceneManager* sceneMng);
	// ゲームシーン破棄
	~SceneGame() override;
	// 毎フレーム更新
	void Update() override;
	// 毎フレーム描画
	void Draw() override;
private:
	// 画面サイズ
	static constexpr int kScreenWidth = 1920;
	static constexpr int kScreenHeight = 1080;
	// プレイヤー画像パス（画像未配置でも動作する）
	static constexpr const char* kPlayerImagePath = "Data/Image/player.png";

	// 入力・プレイヤー・カメラ

	Stage stage_;
	Player player_;
	float cameraX_ = 0.0f;
	FileManager& fileMng_;
	SceneManager* sceneMng_ = nullptr;
	std::shared_ptr<ImageFile> playerImage_;

	// ポーズ中かどうかのフラグ
	bool isPause_ = false;
	int pauseCursor_ = 0; // ポーズメニューのカーソル位置(0:RESUME, 1:TITLE, 2:QUIT)
};
