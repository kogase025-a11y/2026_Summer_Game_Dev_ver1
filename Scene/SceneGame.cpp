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



#include <algorithm>

SceneGame::SceneGame(FileManager& fileMng, SceneManager* sceneMng)
	: player_(&stage_,fileMng), fileMng_(fileMng), sceneMng_(sceneMng)
{
	

	// --- ギミック用の画像をロード ---
	auto waterTex = fileMng_.LoadImageFM("Image/Water.PNG"); // パスは適宜合わせてください
	// 水たまりを追加
	gimmicks_.push_back(std::make_shared<GimmickPuddle>(800.0f, 1200.0f, 760.0f));

	// 水滴を追加
	gimmicks_.push_back(std::make_shared<GimmickWater>(1500.0f, 200.0f, 760.0f, 2.0f, waterTex));


	// ★【追加】シーンマネージャーから、さっき保存したステージ番号（1?3）を受け取る
	int stageNum = sceneMng_->GetStageNum();

	// ★【追加】ステージ番号に応じて、ステージの地形データを切り替える（Init関数は後で作ります）
	stage_.Init(stageNum, fileMng);

	player_.SystemInit();
	player_.GameInit();

	

	// --- アイテムの初期化 ---
	isItemExist_ = false;
	if (stageNum == 1 || stageNum == 2) {
		isItemExist_ = true;
		itemX_ = 700.0f; // とりあえず X=800 の位置に配置
		itemY_ = stage_.GetGroundY() - 50.0f; // 地面から少し浮かす
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

	// --- 【追加】全ギミックの更新と当たり判定 ---
	float deltaTime = 1.0f / 60.0f; // 60FPS固定と仮定
	for (auto& gimmick : gimmicks_) {
		// ギミックの動きを更新（水滴が落ちる等）
		gimmick->Update(deltaTime);

		// プレイヤーとの当たり判定
		if (player_.GetHitBox().IsHit(gimmick->GetHitBox())) {
			// 当たった時の処理（汚れ増加）
			gimmick->OnTouch(player_, deltaTime);
		}
		else {
			// 離れたらリセット
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
	if (isItemExist_) {
		// プレイヤーの当たり判定
		Rect playerRect{ player_.GetX() - 24.0f, player_.GetY() - 48.0f, 48.0f, 48.0f };
		// アイテムの当たり判定（32x32の範囲とする）
		Rect itemRect{ itemX_ - 16.0f, itemY_ - 16.0f, 32.0f, 32.0f };

		if (playerRect.IsHit(itemRect)) {
			isItemExist_ = false; // アイテムを消す

			// ★追加：現在のステージ番号に応じてフラグを立てる
			int stageNum = sceneMng_->GetStageNum();
			if (stageNum == 1) sceneMng_->SetItem1(true);
			if (stageNum == 2) sceneMng_->SetItem2(true);

		}
	}
	// Git Project  Rect gS[?
	const Rect playerRect{ player_.GetX() - 24.0f, player_.GetY() - 48.0f, 48.0f, 48.0f };
	const Rect goalRect{ stage_.GetGoalX() - 16.0f, stage_.GetGroundY() - 180.0f, 32.0f, 180.0f };
	if (playerRect.IsHit(goalRect) && !isGoal_)
	{
		isGoal_ = true;
		goalTimer_ = 0;


		
		player_.PlayGoalSound();
	}
	
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
