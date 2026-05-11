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
	// ESCキーでポーズの切り替え
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE))
	{
		isPause_ = !isPause_;
	}

	// ポーズ中の場合はゲームの更新をストップしてメニュー操作のみ受け付ける
	if (isPause_)
	{
		if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RETURN))
		{
			EndScene(SceneID::TITLE); // Enterでタイトルへ戻る
		}
		if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_Q))
		{
			EndScene(SceneID::EXIT); // Qでゲーム終了
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
		DrawBox(kScreenWidth / 2 - 200, 400, kScreenWidth / 2 + 200, 600, GetColor(50, 50, 150), TRUE);
		DrawString(kScreenWidth / 2 - 80, 440, "PAUSE MENU", GetColor(255, 255, 255));
		DrawString(kScreenWidth / 2 - 120, 490, "Press Enter to TITLE", GetColor(200, 200, 200));
		DrawString(kScreenWidth / 2 - 120, 530, "Press ESC to RESUME", GetColor(200, 200, 200));
		DrawString(kScreenWidth / 2 - 120, 570, "Press Q to QUIT GAME", GetColor(200, 200, 200));
	}
}
