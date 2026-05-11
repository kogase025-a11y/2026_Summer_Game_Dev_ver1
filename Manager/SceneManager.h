#pragma once
#include <memory>
#include "../Scene/SceneSuper.h"
#include "../Util/Fade.h"
class FileManager;


struct ClearResult
{
	// クリアタイム
	float time;
	// ステージ番号
	int stageIndex;
};
class SceneManager
{
public:
	// シーン遷移の進行状態
	enum class TransitionState
	{
		None,
		FadeInNext,
		FadeOutCurrent,
		SwitchScene,
	};

	// シーン管理生成
	SceneManager(FileManager& fileMng);
	~SceneManager();
	// 更新・描画
	void Update();
	void Draw();

	// 終了要求の有無
	bool GetExit();
	// クリア結果を保存
	void SetClearResult(const ClearResult& result);
private:
	// シーン生成切り替え
	void ChangeScene(SceneSuper::SceneID nextSceneID);
	// フェード遷移更新
	void UpdateTransition();
	std::unique_ptr<SceneSuper> currentScene; // 大元のSceneはかならず一つ
	FileManager& fileMng_;

	// アプリ終了要求
	bool isExit;

	// 遷移管理データ
	TransitionState transitionState_ = TransitionState::None;
	SceneSuper::SceneID nextSceneID_ = SceneSuper::SceneID::NONE;
	Fade fade_;

	// クリア結果キャッシュ
	ClearResult clearResult_;
};


