#include "SceneSetting.h"
#include "../Manager/Input/InputManager.h"

SceneSetting::SceneSetting(FileManager&)
{
}

void SceneSetting::Update()
{
	auto& input = InputManager::GetInstance();
	if (input.IsTrgDown(KEY_INPUT_ESCAPE) ||
		input.IsTrgDown(KEY_INPUT_RETURN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
	{
		EndScene(SceneID::STAGE_SELECT);
	}
}

void SceneSetting::Draw()
{
	DrawBox(0, 0, 1920, 1080, GetColor(50, 50, 50), TRUE);
	DrawString(1920 / 2 - 100, 1080 / 2 - 30, "--- SETTING ---", GetColor(255, 255, 255));
	DrawString(1920 / 2 - 160, 1080 / 2 + 20, "PRESS ENTER or ESC TO RETURN", GetColor(200, 200, 200));
}