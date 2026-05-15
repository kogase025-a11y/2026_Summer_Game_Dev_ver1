#include "SceneGame.h"

#include "../Manager/FileManager.h"
#include "../Util/Rect.h"

#include <algorithm>

SceneGame::SceneGame(FileManager& fileMng, SceneManager* sceneMng)
	: player_(&stage_,fileMng), fileMng_(fileMng), sceneMng_(sceneMng)
{
	player_.SystemInit();
	player_.GameInit();

	// Git Project  FileManager g?vC[?擾
	// 画像が未配置でも player_.Draw 側で矩形描画にフォールバックする
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
			if (pauseCursor_ < 0) pauseCursor_ = 2;
		}
		if (downPressed)
		{
			pauseCursor_++;
			if (pauseCursor_ > 2) pauseCursor_ = 0;
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
				EndScene(SceneID::TITLE); // タイトルへ
			}
			else if (pauseCursor_ == 2)
			{
				EndScene(SceneID::EXIT); // 終了
			}
		}
		return;
	}

	// プレイヤー更新
	player_.Update(InputManager::GetInstance());

	// プレイヤー中心にカメラを追従（ステージ外に出ないよう制限）
	const float targetCameraX = player_.GetX() - (kScreenWidth * 0.5f);
	const float cameraMax = stage_.GetStageWidth() - static_cast<float>(kScreenWidth);
	cameraX_ = (std::max)(0.0f, (std::min)(targetCameraX, cameraMax));

	// Git Project  Rect gS[?
	const Rect playerRect{ player_.GetX() - 24.0f, player_.GetY() - 48.0f, 48.0f, 48.0f };
	const Rect goalRect{ stage_.GetGoalX() - 16.0f, stage_.GetGroundY() - 180.0f, 32.0f, 180.0f };
	if (playerRect.IsHit(goalRect))
	{
		EndScene(SceneID::CLEAR);
		return;
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

	// vC[`i? Player ?``j
	const int playerGraphHandle = (playerImage_ ? playerImage_->GetHandle() : -1);
	player_.Draw(cameraX_, playerGraphHandle);

	// UI
	DrawFormatString(20, 20, GetColor(255, 255, 255), "STATE: %s", player_.GetStateName());
	DrawString(20, 48, "LEFT/RIGHT: MOVE  SPACE: JUMP  C: CLEAR  ESC: PAUSE", GetColor(0, 0, 0));

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

		DrawString(kScreenWidth / 2 - 120, 500, (pauseCursor_ == 0 ? "> RESUME" : "  RESUME"), color0);
		DrawString(kScreenWidth / 2 - 120, 540, (pauseCursor_ == 1 ? "> TITLE" : "  TITLE"), color1);
		DrawString(kScreenWidth / 2 - 120, 580, (pauseCursor_ == 2 ? "> QUIT GAME" : "  QUIT GAME"), color2);
	}
}
