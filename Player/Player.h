#pragma once
#include "../Input/InputManager.h"
#include <memory>
#include <vector>
#include "../Application/Application.h"
#include "../Resource/ImageFile.h"
#include "../Util/Rect.h"

class FileManager;
class Vector2;
class SceneGame;
class Stage;

class Player
{
public:

	static constexpr int PlayerWidth = 48; // プレイヤーの幅
	static constexpr int PlayerHeight = 48; // プレイヤーの高さ

	enum class State
	{
		Idle,
		Run,
		Jump,
		Fall,
		Attack,
		Damage
	};
	

	
	// 生成・破棄
	Player(Stage* stage, FileManager& fileMng);
	~Player(void);
	// 初期化・解放
	bool SystemInit(void);
	void GameInit(void);
	void Update(void);
	void Draw(void);
	bool Release(void);

	// 入力付き更新・カメラ付き描画
	void Update(const InputManager& input);
	void Draw(float cameraX, int playerGraphHandle) const;

	// 状態/位置取得
	const char* GetStateName() const;
	float GetX() const;
	float GetY() const;

private:

	SceneGame* sceneGame_;
	FileManager& fileMng_;
	Stage* stage_;


	std::shared_ptr<ImageFile> particleTex;

	// 現在の状態
	float positionX_ = 300.0f;
	float positionY_ = 760.0f;
	float velocityY_ = 0.0f;
	bool onGround_ = true;

	// 移動パラメータ
	const float moveSpeed_ = 7.0f;
	const float gravity_ = 0.65f;
	const float jumpSpeed_ = 14.0f;

	// アニメーション用状態名
	const char* stateName_ = "Idle";
};

