#include "Application.h"

#include <DxLib.h>

#include "../Manager/FileManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Input/InputManager.h"
#include "../Manager/BGMManager.h"
#include "../FPS/Fps.h"

Application::Application()
{
}

Application::~Application()
{
}

bool Application::SystemInit(void)
{
	ChangeWindowMode(TRUE);
	SetGraphMode(SCREEN_SIZE_WID, SCREEN_SIZE_HIG, 32);

	if (DxLib_Init() == -1)
	{
		return false;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	InputManager::CreateInstance();

	fileMng_.reset(new FileManager());
	sceneMana.reset(new SceneManager(*fileMng_));
	fps_.reset(new Fps());

	return true;
}

void Application::Run(void)
{
	while (ProcessMessage() == 0)
	{
		// ポーズメニューを実装するため、ここでのESC強制終了は無効化します
		/*
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0)
		{
			break;
		}
		*/
		if (sceneMana && sceneMana->GetExit())
		{
			break;
		}

		Update();
		Draw();
		ScreenFlip();
		if (fps_)
		{
			fps_->Wait();
		}
	}
}

void Application::Update(void)
{
	if (fps_)
	{
		fps_->Update();
	}

	InputManager::GetInstance().Update();

	if (sceneMana)
	{
		sceneMana->Update();
	}
}

void Application::Draw(void)
{
	ClearDrawScreen();
	if (sceneMana)
	{
		sceneMana->Draw();
	}

	if (fps_)
	{
		fps_->Draw();
	}
}

bool Application::Release(void)
{
	fps_.reset();
	sceneMana.reset();
	fileMng_.reset();
	InputManager::GetInstance().Destroy();
	DxLib_End();
	return true;
}