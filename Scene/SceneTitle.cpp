#include "SceneTitle.h"
#include "../Manager/FileManager.h"
#include "../Manager/Input/InputManager.h" 

#include <cstring>

SceneTitle::SceneTitle(FileManager& fileMng)

	: titleBg_(fileMng.LoadImageFM("./image/GameClear.png"))//タイトルシーン画像の読み込み
{
	
	
	
	
}

void SceneTitle::Update()
{
	auto& input = InputManager::GetInstance();
	// ENTER または コントローラのSTARTキーかAボタンでゲーム開始
	if (input.IsTrgDown(KEY_INPUT_RETURN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		// 決定音の再生
		/*
		if (seDecide_ && seDecide_->GetHandle() != -1) {
			PlaySoundMem(seDecide_->GetHandle(), DX_PLAYTYPE_BACK, TRUE);
		}
		*/
		EndScene(SceneID::STAGE_SELECT);
	}
}

void SceneTitle::Draw()
{
	// タイトル背景とボタン風UIを描画
	const int width = 1920;
	const int height = 1080;
	const int centerX = width / 2;
	auto drawCenterText = [centerX](int y, const char* text, int color)
	{
		const int textW = GetDrawStringWidth(text, static_cast<int>(std::strlen(text)));
		DrawString(centerX - textW / 2, y, text, color);
	};

	DrawBox(0, 0, width, height, GetColor(10, 10, 30), TRUE);
	DrawBox(40, 40, width - 40, height - 40, GetColor(180, 20, 20), FALSE);
	DrawRotaGraphF(centerX, height/2, 1.5, 0.0, titleBg_->GetHandle(), TRUE);
	
	drawCenterText(800, "PRESS ENTER TO START", GetColor(250, 250, 250));


	drawCenterText(850, "PUSH START", GetColor(255, 255, 255));
}
