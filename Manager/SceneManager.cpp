#include "SceneManager.h"
#include <DxLib.h>
#include <algorithm>
#include <memory>

#include "FileManager.h"

#include "../Scene/SceneTitle.h"
#include "../Scene/SceneGame.h"
#include "../Scene/SceneClear.h"

SceneManager::SceneManager(FileManager& fileMng) : fileMng_(fileMng)
{
	currentScene = std::unique_ptr<SceneSuper>(new SceneTitle(fileMng_));
	isExit = false;
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	// 通常状態なら現在シーンを更新
	if (transitionState_ == TransitionState::None)
	{
		currentScene->Update();
		// シーン終了要求が来たらフェードアウトへ
		if (currentScene->IsEnd() && !currentScene->GetIsTransition())
		{
			nextSceneID_ = currentScene->GetNextScene();
			transitionState_ = TransitionState::FadeOutCurrent;
			fade_.Start(Fade::State::Out, 45.0f);
		}
	}
	// 遷移中はフェード処理を更新
	if (transitionState_ != TransitionState::None)
	{
		UpdateTransition();
	}
}

void SceneManager::UpdateTransition()
{
	fade_.Update();

	switch (transitionState_)
	{
	case TransitionState::FadeOutCurrent:
		// 現在シーンをフェードアウト
		currentScene->SetIsTransition(true);
		currentScene->SetTransitionOut(fade_.GetProgress());
		if (fade_.IsFinished())
		{
			transitionState_ = TransitionState::SwitchScene;
		}
		break;
	case TransitionState::SwitchScene:
		// 次シーンに切り替えてフェードイン開始
		ChangeScene(nextSceneID_);
		transitionState_ = TransitionState::FadeInNext;
		fade_.Start(Fade::State::In, 45.0f);
		currentScene->SetTransitionIn(0.0f);
		currentScene->SetIsTransition(true);
		break;
	case TransitionState::FadeInNext:
		// 次シーンをフェードイン
		currentScene->SetTransitionIn(fade_.GetProgress());
		if (fade_.IsFinished())
		{
			transitionState_ = TransitionState::None;
			currentScene->SetIsTransition(false);
		}
	}
}

void SceneManager::Draw()
{
	// 現在シーンの描画
	if (currentScene)
	{
		currentScene->Draw();
	}

	// 遷移中のフェード描画
	if (transitionState_ != TransitionState::None)
	{
		if (transitionState_ == TransitionState::SwitchScene)
		{
			fade_.DrawBlack();
		}
		else
		{
			fade_.Draw();
		}
	}
}

void SceneManager::ChangeScene(SceneSuper::SceneID nextSceneID)
{
	// 指定されたIDのシーンへ切り替える
	switch (nextSceneID)
	{
	case SceneSuper::SceneID::TITLE:
		currentScene = std::unique_ptr<SceneSuper>(new SceneTitle(fileMng_));
		break;
	case SceneSuper::SceneID::GAME:
		currentScene = std::unique_ptr<SceneSuper>(new SceneGame(fileMng_, this));
		break;
	case SceneSuper::SceneID::CLEAR:
		currentScene = std::unique_ptr<SceneSuper>(new SceneClear(fileMng_));
		break;
	case SceneSuper::SceneID::EXIT:
		isExit = true;
		break;
	default:
#ifdef _DEBUG
		OutputDebugStringA("Unknown SceneID\n");
#endif
		break;
	}
}

bool SceneManager::GetExit()
{
	return isExit;
}

void SceneManager::SetClearResult(const ClearResult& result)
{
	clearResult_ = result;
}
