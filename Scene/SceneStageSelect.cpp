#include "SceneStageSelect.h"
#include "../Manager/FileManager.h"
#include "../Manager/Input/InputManager.h"
#include "../Manager/SceneManager.h"
#include <DxLib.h>
#include <math.h>

const int kMaxStages = 4;

SceneStageSelect::SceneStageSelect(FileManager& fileMng,SceneManager* sceneMng)
{
	sceneMng_ = sceneMng;


	explanFontHandle = CreateFontToHandle("Arial", 40, 3, DX_FONTTYPE_ANTIALIASING);
	offScreenHandle_ = MakeScreen(1920, 1080, TRUE);
	live2DScreenHandle_ = -1;
	fadeAlpha_ = 255.0f;

	// 音声の読み込み（ダミーパス、入れ替えて使用してください）
	bgm_ = fileMng.LoadSoundFM("Sound/BGM_Select.wav");
	seCursor_ = fileMng.LoadSoundFM("Sound/SE_Cursor.wav");
	seDecide_ = fileMng.LoadSoundFM("Sound/SE_Decide.wav");

	// BGMの再生
	/*
	if (bgm_ && bgm_->GetHandle() != -1) {
		PlaySoundMem(bgm_->GetHandle(), DX_PLAYTYPE_LOOP);
	}
	*/
}

SceneStageSelect::~SceneStageSelect()
{
	// BGMの停止
	/*
	if (bgm_ && bgm_->GetHandle() != -1) {
		StopSoundMem(bgm_->GetHandle());
	}
	*/

	DeleteFontToHandle(explanFontHandle);
	DeleteGraph(offScreenHandle_);
}

void SceneStageSelect::Update()
{
	auto& input = InputManager::GetInstance();

	// ESCキー または STARTボタン でポーズの切り替え
	if (input.IsTrgDown(KEY_INPUT_ESCAPE) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		isPause_ = !isPause_;
		if (isPause_) pauseCursor_ = 0; // ポーズを開いた時にカーソルをリセット
	}

	// ポーズ画面中の処理
	if (isPause_)
	{
		const int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		
		bool upPressed = input.IsTrgDown(KEY_INPUT_UP) || (padState & PAD_INPUT_UP);
		bool downPressed = input.IsTrgDown(KEY_INPUT_DOWN) || (padState & PAD_INPUT_DOWN);
		
		static int pausePrevPadState = 0;
		if (padState & PAD_INPUT_UP && (pausePrevPadState & PAD_INPUT_UP)) upPressed = false;
		if (padState & PAD_INPUT_DOWN && (pausePrevPadState & PAD_INPUT_DOWN)) downPressed = false;
		pausePrevPadState = padState;

		if (upPressed)
		{
			pauseCursor_--;
			if (pauseCursor_ < 0) pauseCursor_ = 2;
			/*
			if (seCursor_ && seCursor_->GetHandle() != -1) {
				PlaySoundMem(seCursor_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
			}
			*/
		}
		if (downPressed)
		{
			pauseCursor_++;
			if (pauseCursor_ > 2) pauseCursor_ = 0;
			/*
			if (seCursor_ && seCursor_->GetHandle() != -1) {
				PlaySoundMem(seCursor_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
			}
			*/
		}

		if (input.IsTrgDown(KEY_INPUT_RETURN) ||
			input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
			input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
		{
			/*
			if (seDecide_ && seDecide_->GetHandle() != -1) {
				PlaySoundMem(seDecide_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
			}
			*/

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

	UpdateInput();
	UpdateAnimation();
}

void SceneStageSelect::Draw()
{
	DrawBox(0, 0, 1920, 1080, GetColor(30, 30, 80), TRUE);

	DrawRing(ringCenterX_, ringCenterY_);
	DrawStageName(stageNameTextCenterX_, stageNameTextCenterY_);
	DrawArrow(ringCenterX_, ringCenterY_);

	// 左上に操作説明UIを描画
	DrawFormatString(20, 20, GetColor(255, 255, 255), "UP/DOWN: Select   ENTER/A: Start");
	DrawFormatString(20, 48, GetColor(200, 200, 200), "ESC/START: Pause Menu");

	// ポーズ画面描画
	if (isPause_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawBox(1920 / 2 - 200, 400, 1920 / 2 + 200, 650, GetColor(50, 50, 150), TRUE);
		DrawString(1920 / 2 - 80, 440, "PAUSE MENU", GetColor(255, 255, 255));

		// カーソルの位置に応じて色を変える
		int color0 = (pauseCursor_ == 0) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
		int color1 = (pauseCursor_ == 1) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
		int color2 = (pauseCursor_ == 2) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);

		DrawString(1920 / 2 - 120, 500, (pauseCursor_ == 0 ? "> RESUME" : "  RESUME"), color0);
		DrawString(1920 / 2 - 120, 540, (pauseCursor_ == 1 ? "> TITLE" : "  TITLE"), color1);
		DrawString(1920 / 2 - 120, 580, (pauseCursor_ == 2 ? "> QUIT GAME" : "  QUIT GAME"), color2);
	}
}

void SceneStageSelect::SetTransitionIn(float t)
{
	SceneSuper::SetTransitionIn(t);
}

void SceneStageSelect::SetTransitionOut(float t)
{
	SceneSuper::SetTransitionOut(t);
}

void SceneStageSelect::UpdateInput()
{
	if (animState_ != SelectAnimState::Idle) return;

	auto& input = InputManager::GetInstance();
	const int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	
	static int prevPadState = 0;
	bool upPressed = input.IsTrgDown(KEY_INPUT_UP) || ((padState & PAD_INPUT_UP) && !(prevPadState & PAD_INPUT_UP));
	bool downPressed = input.IsTrgDown(KEY_INPUT_DOWN) || ((padState & PAD_INPUT_DOWN) && !(prevPadState & PAD_INPUT_DOWN));
	prevPadState = padState;

	if (upPressed)
	{
		nextStageIndex_ = selectedStageIndex_ - 1;
		if (nextStageIndex_ < 1) nextStageIndex_ = kMaxStages;
		rotationDir_ = 1;
		/*
		if (seCursor_ && seCursor_->GetHandle() != -1) {
			PlaySoundMem(seCursor_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
		*/
		StartTransition();
	}
	else if (downPressed)
	{
		nextStageIndex_ = selectedStageIndex_ + 1;
		if (nextStageIndex_ > kMaxStages) nextStageIndex_ = 1;
		rotationDir_ = -1;
		/*
		if (seCursor_ && seCursor_->GetHandle() != -1) {
			PlaySoundMem(seCursor_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
		*/
		StartTransition();
	}
	else if (input.IsTrgDown(KEY_INPUT_RETURN) ||
			 input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) ||
			 input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		/*
		if (seDecide_ && seDecide_->GetHandle() != -1) {
			PlaySoundMem(seDecide_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
		*/
		if (selectedStageIndex_ == 4)
		{
			EndScene(SceneID::SETTING);
		}
		else
		{
			
			// ちゃんと中身があれば、今まで通り保存する
			sceneMng_->SetStageNum(selectedStageIndex_);
			
			EndScene(SceneID::GAME);
		}
	}
}

void SceneStageSelect::UpdateAnimation()
{
	if (animState_ == SelectAnimState::Transition)
	{
		animTimer_ += 0.05f;
		if (animTimer_ >= 1.0f)
		{
			animTimer_ = 1.0f;
			animState_ = SelectAnimState::Idle;
			selectedStageIndex_ = nextStageIndex_;
			drawingStageIndex_ = selectedStageIndex_;
			ringBaseAngle_ += rotationDir_ * (DX_PI_F * 2.0f / kMaxStages);
			ringAnimAngle_ = 0.0f;
		}
		else
		{
			// イージング(Ease Out)
			float t = animTimer_;
			float easeOut = 1.0f - powf(1.0f - t, 3.0f);
			ringAnimAngle_ = rotationDir_ * (DX_PI_F * 2.0f / kMaxStages) * easeOut;
		}
	}
	else
	{
		drawingStageIndex_ = selectedStageIndex_;
	}
	
	// 矢印のアニメーション
	arrowAnimTimer_ += arrowAnimSpeed_;
	arrowOffset_ = sinf(arrowAnimTimer_) * 10.0f;
}

void SceneStageSelect::StartTransition()
{
	animState_ = SelectAnimState::Transition;
	animTimer_ = 0.0f;
}

void SceneStageSelect::UpdateRingParams(float angle)
{
}

void SceneStageSelect::UpdateStageNameParams(float angle)
{
}

void SceneStageSelect::UpdateArrowParams()
{
}

void SceneStageSelect::DrawRing(int ox, int oy)
{
	// 簡易的なリング回転の描画
	float currentAngle = ringBaseAngle_ + ringAnimAngle_;
	
	// 中央（960, 540）を中心に円を描画する
	int cx = 1920 / 2;
	int cy = 1080 / 2;

	for (int i = 0; i < kMaxStages; i++)
	{
		// 要素ごとの角度（0が真下になるように + DX_PI_F/2 等で調整）
		float angle = currentAngle + (i * DX_PI_F * 2.0f / kMaxStages) - (DX_PI_F / 2.0f);
		int x = cx + (int)(cosf(angle) * 350.0f);
		int y = cy + (int)(sinf(angle) * 350.0f);
		
		int displayIndex = (kMaxStages - i + selectedStageIndex_ - 1) % kMaxStages + 1;

		bool isSelected = ((i + 1) == drawingStageIndex_ && animState_ == SelectAnimState::Idle);
		
		if (isSelected) 
		{ 
			SetDrawBlendMode(DX_BLENDMODE_ADD, 128 + (int)(sinf(arrowAnimTimer_ * 5.0f) * 127)); 
			DrawCircle(x, y, 75, GetColor(255, 255, 100), TRUE); 
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); 
		} 
		
		int color = isSelected ? GetColor(255, 255, 0) : GetColor(150, 150, 150);
		DrawCircle(x, y, 60, color, TRUE); 
		
		if (i + 1 == 4)
		{
			DrawFormatString(x - 30, y - 10, GetColor(0, 0, 0), "SETTING");
		}
		else
		{
			DrawFormatString(x - 20, y - 10, GetColor(0, 0, 0), "STG %d", i + 1);
		}
	}
}

void SceneStageSelect::DrawStageName(int ox, int oy)
{
	DrawStringToHandle(1920 / 2 - 140, 1080 / 2 - 60, "STAGE SELECT", GetColor(255, 255, 255), explanFontHandle);
	if (drawingStageIndex_ == 4)
	{
		DrawFormatStringToHandle(1920 / 2 - 80, 1080 / 2, GetColor(255, 255, 0), explanFontHandle, "SETTING");
	}
	else
	{
		DrawFormatStringToHandle(1920 / 2 - 80, 1080 / 2, GetColor(255, 255, 0), explanFontHandle, "STAGE %d", drawingStageIndex_);
	}
}

void SceneStageSelect::DrawArrow(int ox, int oy)
{
	int cx = 1920 / 2;
	int cy = 1080 / 2;
	
	// 上矢印
	int topY = cy - 200 - (int)arrowOffset_;
	DrawTriangle(cx, topY - 30, cx - 40, topY + 20, cx + 40, topY + 20, GetColor(200, 200, 200), TRUE);
	
	// 下矢印
	int btmY = cy + 200 + (int)arrowOffset_;
	DrawTriangle(cx, btmY + 30, cx - 40, btmY - 20, cx + 40, btmY - 20, GetColor(200, 200, 200), TRUE);
}



