#include "SceneClear.h"


#include "../Manager/FileManager.h"
#include "../Manager/Input/InputManager.h"
#include "../Resource/ImageFile.h"
#include <cstring>

SceneClear::SceneClear(FileManager& fileMng)
	: clearBg_(fileMng.LoadImageFM("Image/GameCliar.PNG")) // ここでクリア画面の画像を読み込みます
	, scoreXStart_(700.0f) // スコアの開始X座標
	, scoreXEnd_(1200.0f)// スコアの終了X座標
	, scoreYTop_(540.0f)   // スコアのY座標
{}

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


	const int scoreleft = static_cast<int>(scoreXStart_);
	const int scoreRight = static_cast<int>(scoreXEnd_);
	const int scoreTop = static_cast<int>(scoreYTop_);
	DrawBox(scoreleft, scoreTop, scoreRight, scoreTop + 100, GetColor(255, 255, 255), TRUE);

	
	

	

}

float SceneClear::GetScoreY() const { return scoreYTop_; }
float SceneClear::GetScoreXStart() const { return scoreXStart_; }
float SceneClear::GetScoreXEnd() const { return scoreXEnd_; }
float SceneClear::GetScoreYTop() const { return scoreYTop_; }

float SceneClear::GetScoreYAtX(float x) const
{
	if (x >= scoreXStart_ && x <= scoreXEnd_)
	{
		return scoreYTop_;
	}
	return scoreYTop_;
}