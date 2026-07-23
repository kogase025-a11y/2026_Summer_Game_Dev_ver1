#include <DxLib.h>
#include"SceneGame.h"

#include "../Manager/FileManager.h"
#include "../Util/Rect.h"
#include "../Manager/Input/InputManager.h"
#include "../Player/Player.h"
#include "../Stage/Stage.h"
#include"../Manager/SceneManager.h"
#include "../Gimmick/GimmickPuddle.h"
#include "../Gimmick/GimmickWaterDrop.h" 
#include "../Gimmick/GimmickSplash.h"
#include "../Gimmick/GimmickRain.h"

#include <algorithm>

SceneGame::SceneGame(FileManager& fileMng, SceneManager* sceneMng)
	: player_(&stage_, fileMng), fileMng_(fileMng), sceneMng_(sceneMng)
{


	int stageNum = sceneMng_->GetStageNum();
	auto waterTex = fileMng_.LoadImageFM("Image/Water.PNG");

	// 地面の高さ定数 (12マス目 * 64px = 768px)
	const float floorY = 768.0f;

	// --- ステージ番号ごとのギミック配置 ---
	if (stageNum == 1) {
		// 地面(floorY)の上に重なるようにギミックを配置
		gimmicks_.push_back(std::make_shared<GimmickPuddle>(800.0f, 1200.0f, floorY));
		gimmicks_.push_back(std::make_shared<GimmickWater>(1500.0f, 200.0f, floorY, 2.0f, waterTex));
		gimmicks_.push_back(std::make_shared<GimmickSplash>(900.0f, floorY - 50.0f, 100.0f, 50.0f));

		// 追加：雨を降らせるテスト（X:0～3000、Y:0から開始、幅3000、高さ1000の範囲で雨を降らせます）
		gimmicks_.push_back(std::make_shared<GimmickRain>(0.0f, 0.0f, 3000, 1000, this));
	}
	else if (stageNum == 2) {
		gimmicks_.push_back(std::make_shared<GimmickPuddle>(1000.0f, 1500.0f, floorY));
		gimmicks_.push_back(std::make_shared<GimmickSplash>(1200.0f, floorY - 50.0f, 100.0f, 50.0f));
	}
	else if (stageNum == 3) {
		gimmicks_.push_back(std::make_shared<GimmickWater>(800.0f, 200.0f, floorY, 1.5f, waterTex));
		gimmicks_.push_back(std::make_shared<GimmickWater>(1200.0f, 200.0f, floorY, 2.5f, waterTex));
	}

	stage_.Init(stageNum, fileMng);

	player_.SystemInit();
	player_.GameInit(); // 初期Y座標は内部で 768.0f に設定されます

	// --- アイテムの初期化 ---
	isItemExist_ = false;
	if (stageNum == 1 || stageNum == 2) {
		isItemExist_ = true;
		itemX_ = 700.0f;
		itemY_ = floorY - 50.0f; // 地面から少し浮かす
	}


	// ★追加：ステージ3開始時にアイテムチェック
	if (stageNum == 3) {
		if (sceneMng_->GetItem1() && sceneMng_->GetItem2()) {
			player_.StartInvincible(10.0f); // 10秒無敵！
		}
	}


	playerImage_ = fileMng_.LoadImageFM(kPlayerImagePath);
}

SceneGame::~SceneGame() = default;

void SceneGame::Update()
{
	auto& input = InputManager::GetInstance();

	// ESCキー または コントローラのSTARTボタン でポーズの切り替え
	if (input.IsTrgDown(KEY_INPUT_ESCAPE) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		isPause_ = !isPause_;
		if (isPause_) pauseCursor_ = 0; // ポーズを開いた時にカーソルをリセット
	}

	// ポーズ画面中の処理
	if (isPause_)
	{
		// 上下入力の判定(キーボード & パッド)
		const int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		
		bool upPressed = input.IsTrgDown(KEY_INPUT_UP) || (padState & PAD_INPUT_UP);
		bool downPressed = input.IsTrgDown(KEY_INPUT_DOWN) || (padState & PAD_INPUT_DOWN);
		
		// 1フレームだけ反応させるために前回入力をチェックする簡易処理
		static int prevPadState = 0;
		if (padState & PAD_INPUT_UP && (prevPadState & PAD_INPUT_UP)) upPressed = false;
		if (padState & PAD_INPUT_DOWN && (prevPadState & PAD_INPUT_DOWN)) downPressed = false;
		prevPadState = padState;

		if (upPressed)
		{
			pauseCursor_--;
			if (pauseCursor_ < 0) pauseCursor_ = 3;
		}
		if (downPressed)
		{
			pauseCursor_++;
			if (pauseCursor_ > 3) pauseCursor_ = 0;
		}

		// ENTERキー または Bボタン(RIGHT), Aボタン(DOWN) で決定
		if (input.IsTrgDown(KEY_INPUT_RETURN) ||
			input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
			input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			if (pauseCursor_ == 0)
			{
				isPause_ = false; // 再開
			}
			else if (pauseCursor_ == 1)
			{
				EndScene(SceneID::STAGE_SELECT); // ステージセレクトへ
			}
			else if (pauseCursor_ == 2)
			{
				EndScene(SceneID::TITLE); // タイトルへ
			}
			else if (pauseCursor_ == 3)
			{
				EndScene(SceneID::EXIT); // 終了
			}
		}
		return;
	}

	// プレイヤー更新
	player_.Update(InputManager::GetInstance());

	// --- 1. 全ギミックの更新と当たり判定 ---
	float deltaTime = 1.0f / 60.0f;
	for (auto& gimmick : gimmicks_) {
		gimmick->Update(deltaTime);
		// ★ player_.GetHitBox() を使っているのでここはOK！
		if (player_.GetHitBox().IsHit(gimmick->GetHitBox())) {
			gimmick->OnTouch(player_, deltaTime);
		}
		else {
			gimmick->OnLeave();
		}
	}


	// 汚れレベルが3以上になったら即座にゲームオーバーシーンへ
	if (player_.GetDirtLevel() >= 3)
	{
		EndScene(SceneID::GAMEOVER);
		return;
	}


	// プレイヤー中心にカメラを追従（ステージ外に出ないよう制限）
	const float targetCameraX = player_.GetX() - (kScreenWidth * 0.5f);
	const float cameraMax = stage_.GetStageWidth() - static_cast<float>(kScreenWidth);
	cameraX_ = (std::max)(0.0f, (std::min)(targetCameraX, cameraMax));
	// --- アイテムとの当たり判定 ---
	  // --- 2. アイテムとの当たり判定 ---
	if (isItemExist_) {
		// ★修正：古い Rect を消して、でかい判定を使う
		Rect playerHit = player_.GetHitBox();
		Rect itemRect{ itemX_ - 20.0f, itemY_ - 20.0f, 40.0f, 40.0f }; // アイテム側の箱

		if (playerHit.IsHit(itemRect)) {
			isItemExist_ = false; // ゲット！
			int stageNum = sceneMng_->GetStageNum();
			if (stageNum == 1) sceneMng_->SetItem1(true);
			if (stageNum == 2) sceneMng_->SetItem2(true);
		}
	}

	// --- 3. ゴールとの当たり判定 ---
	const Rect playerHitForGoal = player_.GetHitBox();

	// ★修正：GetGroundY() [2000px] ではなく、
	// ゴールの場所にある「本当の地面の高さ」を取得します。
	float goalGroundY = stage_.GetGroundYAtX(stage_.GetGoalX());

	// 地面が見つからない場合の保険（念のため）
	if (goalGroundY >= 2000.0f) goalGroundY = 768.0f;

	// 当たり判定の箱(Rect)を地面の上に作成
	// x: ゴールの中心から少し左
	// y: 地面(goalGroundY)から上に 300px 分
	// w: 横幅 100px
	// h: 縦幅 300px
	const Rect goalRect{
		stage_.GetGoalX() - 50.0f,
		goalGroundY - 300.0f,
		100.0f,
		300.0f
	};

	if (playerHitForGoal.IsHit(goalRect) && !isGoal_)
	{
		isGoal_ = true;
		goalTimer_ = 0;
		player_.PlayGoalSound();
	}

	// 【デバッグ：当たり判定を見えるようにする】
	// ゴール付近に「赤い枠」が表示されます。これに触れればクリアです。
	DrawBox(
		static_cast<int>(goalRect.x - cameraX_),
		static_cast<int>(goalRect.y),
		static_cast<int>(goalRect.x + goalRect.w - cameraX_),
		static_cast<int>(goalRect.y + goalRect.h),
		GetColor(255, 0, 0), FALSE
	);

	
	if (isGoal_)
	{
		goalTimer_++;

		if (goalTimer_ > 120)
		{
			EndScene(SceneID::CLEAR);
			return;
		}
	}
	// 終了
	if (CheckHitKey(KEY_INPUT_C) != 0)
	{
		EndScene(SceneID::CLEAR);
		return;
	}
	// 古いESC修了処理は削除
}

void SceneGame::Draw()
{
	// wi
	DrawBox(0, 0, kScreenWidth, kScreenHeight, GetColor(80, 170, 255), TRUE);

	// ステージ（背景・床・段差・ゴール等）の描画
	stage_.Draw(cameraX_, kScreenWidth, kScreenHeight);

	// --- 【追加】全ギミックの描画 ---
	for (auto& gimmick : gimmicks_) {
		gimmick->Draw(static_cast<int>(cameraX_), 0);
	}

	// プレイヤー描画
	const int playerGraphHandle = (playerImage_ ? playerImage_->GetHandle() : -1);
	player_.Draw(cameraX_, playerGraphHandle);

	// UI
	DrawFormatString(20, 20, GetColor(255, 255, 255), "STATE: %s", player_.GetStateName());
	DrawString(20, 48, "LEFT/RIGHT: MOVE  SPACE: JUMP  C: CLEAR  ESC: PAUSE", GetColor(0, 0, 0));
	DrawFormatString(20, 80, GetColor(255, 255, 255), "Item1: %s  Item2: %s",
		sceneMng_->GetItem1() ? "GET!" : "NONE",
		sceneMng_->GetItem2() ? "GET!" : "NONE");
	// --- アイテムの描画 ---
	if (isItemExist_) {
		// ふわふわさせるためのオフセット計算（時間でサイン波を作る）
		float offset = sinf(GetNowCount() / 200.0f) * 10.0f;

		int drawItemX = static_cast<int>(itemX_ - cameraX_);
		int drawItemY = static_cast<int>(itemY_ + offset);

		// 黄色い円を描く（仮のアイテム）
		DrawCircle(drawItemX, drawItemY, 20, GetColor(255, 255, 0), TRUE);
		// 真ん中に「★」とか「ITEM」とか書いておくと分かりやすいです
		DrawString(drawItemX - 15, drawItemY - 8, "ITEM", GetColor(0, 0, 0));
	}




	// ポーズ画面描画
	if (isPause_)
	{
		// 背景を半透明の黒で暗くする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawBox(0, 0, kScreenWidth, kScreenHeight, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// メニューの枠と文字を描画
		DrawBox(kScreenWidth / 2 - 200, 400, kScreenWidth / 2 + 200, 650, GetColor(50, 50, 150), TRUE);
		DrawString(kScreenWidth / 2 - 80, 440, "PAUSE MENU", GetColor(255, 255, 255));

		// カーソルの位置に応じて色を変える
		int color0 = (pauseCursor_ == 0) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
		int color1 = (pauseCursor_ == 1) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
		int color2 = (pauseCursor_ == 2) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
		int color3 = (pauseCursor_ == 3) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);

		DrawString(kScreenWidth / 2 - 120, 480, (pauseCursor_ == 0 ? "> RESUME" : "  RESUME"), color0);
		DrawString(kScreenWidth / 2 - 120, 520, (pauseCursor_ == 1 ? "> TO STAGE SELECT" : "  TO STAGE SELECT"), color1);
		DrawString(kScreenWidth / 2 - 120, 560, (pauseCursor_ == 2 ? "> TO TITLE" : "  TO TITLE"), color2);
		DrawString(kScreenWidth / 2 - 120, 600, (pauseCursor_ == 3 ? "> QUIT GAME" : "  QUIT GAME"), color3);
	}
}

bool SceneGame::CheckStageCollision(float x, float y) const {
	// ステージの壁などに当たっているか（雨用の判定）
	if (stage_.IsWall(x, y)) {
		return true;
	}
	if (stage_.IsBlock(x, y)) {
		return true;
	}
	return false;
}

Rect SceneGame::GetPlayerHitBox() const {
	return player_.GetHitBox();
}

void SceneGame::OnPlayerHitRain() {
	// 雨に当たったらゲームオーバーにする
	EndScene(SceneID::GAMEOVER);
}
