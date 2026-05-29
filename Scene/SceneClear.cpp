#include "SceneClear.h"

#include "SceneClear.h"
#include "../Manager/FileManager.h"
#include "../Manager/Input/InputManager.h"
#include "../Resource/ImageFile.h"
#include <cstring>

SceneClear::SceneClear(FileManager& fileMng)
	: clearBg_(fileMng.LoadImageFM("Image/GameCliar.PNG")) // ここでクリア画面の画像を読み込みます
{
}

void SceneClear::Update()
{
	// ENTERでタイトルへ戻る
	auto& input = InputManager::GetInstance();
	if (input.IsTrgDown(KEY_INPUT_RETURN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		EndScene(SceneID::TITLE);
	}
}

void SceneClear::Draw()
{
	const int width = 1920;
	const int height = 1080;
	const int centerX = width / 2;

	// 背景のベタ塗り
	DrawBox(0, 0, width, height, GetColor(10, 10, 30), TRUE);

	// クリア画像の描画
	if (clearBg_ && clearBg_->GetHandle() != -1)
	{
		DrawRotaGraphF(centerX, height / 2.0f, 1.5, 0.0, clearBg_->GetHandle(), TRUE);
	}

	// クリア表示（文字）が必要な場合はこのまま残します
	DrawString(20, 110, "CLEAR: PRESS ENTER to TITLE", GetColor(0, 255, 0));
}
