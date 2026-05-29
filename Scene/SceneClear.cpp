#include "SceneClear.h"

#include "../Manager/FileManager.h"
#include "../Manager/Input/InputManager.h"

SceneClear::SceneClear(FileManager& fileMng)
	
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
	// クリア表示
	DrawString(20, 110, "CLEAR: PRESS ENTER", GetColor(0, 255, 0));
}
