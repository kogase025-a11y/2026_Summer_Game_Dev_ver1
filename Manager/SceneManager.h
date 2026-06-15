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


	// 外部からステージ番号を設定するための関数（セッター）
	void SetStageNum(int num) { currentStageNum_ = num; }

	// 外部からステージ番号を受け取るための関数（ゲッター）
	int GetStageNum() const { return currentStageNum_; }

	void SetItem1(bool flag) { hasItemStage1_ = flag; }
	void SetItem2(bool flag) { hasItemStage2_ = flag; }

	bool GetItem1() const { return hasItemStage1_; }
	bool GetItem2() const { return hasItemStage2_; }


private:

	// SceneManager.h の中の、class SceneManager の定義内に追加
	int currentStageNum_ = 1; // 最初はとりあえず1にしておく

	// シーン生成切り替え
	void ChangeScene(SceneSuper::SceneID nextSceneID);
	// フェード遷移更新
	void UpdateTransition();
	std::unique_ptr<SceneSuper> currentScene; // 大元のSceneはかならず一つ
	FileManager& fileMng_;

	// アプリ終了要求
	bool isExit;

	bool hasItemStage1_ = false; // ステージ1のアイテムを取ったか
	bool hasItemStage2_ = false; // ステージ2のアイテムを取ったか


	// 遷移管理データ
	TransitionState transitionState_ = TransitionState::None;
	SceneSuper::SceneID nextSceneID_ = SceneSuper::SceneID::NONE;
	Fade fade_;

	// クリア結果キャッシュ
	ClearResult clearResult_;
};


