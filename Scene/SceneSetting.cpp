#include "SceneSetting.h"
#include "../Manager/Input/InputManager.h"

SceneSetting::SceneSetting(FileManager&)
{
}

void SceneSetting::Update()
{
	auto& input = InputManager::GetInstance();
	const int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	bool upPressed = input.IsTrgDown(KEY_INPUT_UP) || (padState & PAD_INPUT_UP);
	bool downPressed = input.IsTrgDown(KEY_INPUT_DOWN) || (padState & PAD_INPUT_DOWN);
	bool leftPressed = input.IsTrgDown(KEY_INPUT_LEFT) || (padState & PAD_INPUT_LEFT);
	bool rightPressed = input.IsTrgDown(KEY_INPUT_RIGHT) || (padState & PAD_INPUT_RIGHT);
	bool leftHeld = input.IsNew(KEY_INPUT_LEFT) || (padState & PAD_INPUT_LEFT);
	bool rightHeld = input.IsNew(KEY_INPUT_RIGHT) || (padState & PAD_INPUT_RIGHT);

	if (padState & PAD_INPUT_UP && (prevPadState_ & PAD_INPUT_UP)) upPressed = false;
	if (padState & PAD_INPUT_DOWN && (prevPadState_ & PAD_INPUT_DOWN)) downPressed = false;

	// For volume hold (with delay/repeat or just simple Trg)
	// We'll just use basic repeat manually or only Trg
	if (padState & PAD_INPUT_LEFT && (prevPadState_ & PAD_INPUT_LEFT)) leftPressed = false;
	if (padState & PAD_INPUT_RIGHT && (prevPadState_ & PAD_INPUT_RIGHT)) rightPressed = false;

	prevPadState_ = padState;

	if (upPressed)
	{
		cursorIndex_--;
		if (cursorIndex_ < 0) cursorIndex_ = 2; // 3 items (0,1,2)
	}
	if (downPressed)
	{
		cursorIndex_++;
		if (cursorIndex_ > 2) cursorIndex_ = 0;
	}

	// Adjust Volume
	if (cursorIndex_ == 0)
	{
		if (leftHeld && bgmVolume_ > 0) bgmVolume_--;
		if (rightHeld && bgmVolume_ < 100) bgmVolume_++;
	}
	else if (cursorIndex_ == 1)
	{
		if (leftHeld && seVolume_ > 0) seVolume_--;
		if (rightHeld && seVolume_ < 100) seVolume_++;
	}

	bool enterPressed = input.IsTrgDown(KEY_INPUT_RETURN) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT) ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (input.IsTrgDown(KEY_INPUT_ESCAPE) || (enterPressed && cursorIndex_ == 2))
	{
		EndScene(SceneID::STAGE_SELECT); // or whatever previous scene, we just use STAGE_SELECT here
	}
}

void SceneSetting::Draw()
{
	DrawBox(0, 0, 1920, 1080, GetColor(30, 30, 50), TRUE);

	int oldFontSize = GetFontSize();
	SetFontSize(48);

	// Title
	DrawString(1920 / 2 - 180, 200, "S E T T I N G S", GetColor(255, 255, 255));

	SetFontSize(24);
	// Helper strings
	DrawString(1920 / 2 - 350, 900, "UP / DOWN : SELECT   LEFT / RIGHT : ADJUST   ENTER : EXECUTE", GetColor(150, 150, 150));

	SetFontSize(40);
	int colorBgm  = (cursorIndex_ == 0) ? GetColor(255, 255, 50) : GetColor(200, 200, 200);
	int colorSe   = (cursorIndex_ == 1) ? GetColor(255, 255, 50) : GetColor(200, 200, 200);
	int colorBack = (cursorIndex_ == 2) ? GetColor(255, 255, 50) : GetColor(200, 200, 200);

	DrawString(400, 400, (cursorIndex_ == 0 ? "> BGM VOLUME" : "  BGM VOLUME"), colorBgm);
	DrawString(400, 500, (cursorIndex_ == 1 ? "> SE  VOLUME" : "  SE  VOLUME"), colorSe);
	DrawString(400, 600, (cursorIndex_ == 2 ? "> BACK" : "  BACK"), colorBack);

	// Draw volume bars
	// BGM Vol
	DrawBox(800, 395, 800 + 400, 435, GetColor(100, 100, 100), TRUE);
	DrawBox(800, 395, 800 + (bgmVolume_ * 4), 435, (cursorIndex_ == 0 ? GetColor(0, 200, 255) : GetColor(0, 100, 150)), TRUE);
	DrawFormatString(1250, 400, colorBgm, "%d", bgmVolume_);

	// SE Vol
	DrawBox(800, 495, 800 + 400, 535, GetColor(100, 100, 100), TRUE);
	DrawBox(800, 495, 800 + (seVolume_ * 4), 535, (cursorIndex_ == 1 ? GetColor(0, 200, 255) : GetColor(0, 100, 150)), TRUE);
	DrawFormatString(1250, 500, colorSe, "%d", seVolume_);

	SetFontSize(oldFontSize);
}