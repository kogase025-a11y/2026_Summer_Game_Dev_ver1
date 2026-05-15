#pragma once
#include "../Input/InputManager.h"
#include <memory>
#include <vector>
#include"../Util/Vector2.h"
#include "../Util/Vector2F.h"
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

	static const int SIZE_X = 96;	// 横サイズ
	static const int SIZE_Y = 64;	// 縦サイズ

	// 最大アニメーション数(走るモーションのみ)
	//static const int MAX_NUM_ANIM = 4;

	// アニメーションスピード
	static constexpr float ANIM_SPEED = 0.1f;

	// 移動速度の最大値
	static constexpr float MAX_MOVE_SPEED = 10.5f;

	// 加速(accelerator=アクセレレーター)
	static constexpr float MOVE_ACC = 0.25f;

	// 減速(decelerate=ディセラレイト)
	static constexpr float MOVE_DEC = 0.05f;
	// ジャンプの最大速度
	static constexpr float MAX_JUMP_POW = 8.0f;
	//重力
	static constexpr float GRAVITY = 0.25f;
	// ジャンプ入力フレーム
	static constexpr int INPUT_JUMP_FRAME = 6;
	// アニメーション状態
	enum class ANIM_STATE
	{
		IDLE,
		RUN,
		JUMP,
		DAMAGED,
		MAX
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

	// 座標
	Vector2F pos_;

	// アニメーション状態
	ANIM_STATE animState_;


	// プレイヤーの移動操作
	void ProcessMove(const InputManager& input);

	// 加速(スピードを加える)
	void Accele(float speed);

	// 減速(ディセラレイト)
	void Decelerate(float speed);

	// 移動(実際の座標移動)
	void Move(void);
	// 重力をかける
	void AddGravity(void);
	// プレイヤーのジャンプ操作
	void ProcessJump(const InputManager& input);
	// ジャンプ
	void Jump(void);
	// ジャンプ力の設定
	void SetJumpPow(float pow);

	std::shared_ptr<ImageFile> particleTex;
	std::shared_ptr<ImageFile> wetTexs[3]; // 水たまりに入った時の画像(3種類)

	// 現在の状態
	float positionX_ = 300.0f;
	float positionY_ = 760.0f;
	float velocityX_ = 0.0f;
	float velocityY_ = 0.0f;
	bool onGround_ = true;
	bool isInPuddle_ = false; // 水たまり判定フラグ
	bool wasInPuddle_ = false; // 前回水たまりにいたかのフラグ
	int dirtLevel_ = 0;       // 汚れ段階 (0: 新品, 1?3: 汚れ)
	int jumpTimer_ = 0;

	// 移動パラメータ
	const float moveSpeed_ = 7.0f;
	const float gravity_ = 0.65f;
	const float jumpSpeed_ = 14.0f;

	// アニメーション用状態名
	const char* stateName_ = "Idle";
};

