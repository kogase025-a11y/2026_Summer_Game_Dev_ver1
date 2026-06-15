#include "SceneGameOver.h"
#include "../Manager/FileManager.h"
#include "../Manager/Input/InputManager.h"
#include "../Resource/ImageFile.h"
#include <DxLib.h>
#include"../Manager/SceneManager.h"
#include <cstring>
SceneGameOver::SceneGameOver(FileManager& fileMng)
{
	gameOverBg_ = fileMng.LoadImageFM("Image/GameOver.PNG");

}
void SceneGameOver::Update()
{
	auto& input = InputManager::GetInstance();
	if (input.IsTrgDown(KEY_INPUT_RETURN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		EndScene(SceneID::TITLE);
	}
}
void SceneGameOver::Draw()
{
	const int width = 1920;
	const int height = 1080;
	const int centerX = width / 2;


	DrawRotaGraphF(centerX, height / 2, 1.5, 0.0, gameOverBg_ ? gameOverBg_->GetHandle() : -1, TRUE);


}
