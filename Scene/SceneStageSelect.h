#pragma once
#include <vector>

#include "../Application/Application.h"
#include "SceneSuper.h"
#include "../Resource/ImageFile.h"
#include "../Resource/SoundFile.h"
#include "../Manager/FileManager.h"

class SceneStageSelect : public SceneSuper
{
public:
	
	enum class SelectAnimState
	{
		Idle,
		Transition,
	};

	enum class ArrowPulse
	{
		None,
		Up,
		Down
	};

	SceneStageSelect(FileManager& fileMng);

	~SceneStageSelect() override;

	void Update() override;
	void Draw() override;

	void SetTransitionIn(float t) override;
	void SetTransitionOut(float t) override;

private:
	

	void UpdateInput();
	void UpdateAnimation();
	void StartTransition();
	void UpdateRingParams(float angle);
	void UpdateStageNameParams(float angle);
	void UpdateArrowParams();

	void DrawRing(int ox, int oy);
	void DrawStageName(int ox, int oy);
	void DrawArrow(int ox, int oy);

	

	// 0で扱わず、1から始めることで、ステージ番号とインデックスを一致させる
	// 配列で扱うときは、-1してアクセスする
	int selectedStageIndex_ = 1;
	int nextStageIndex_ = 1;
	int drawingStageIndex_ = 1;


	int ringCenterX_ = 0; // リングの中心X
	int ringCenterY_ = 540; // リングの中心Y
	int stageNameTextCenterX_ = ringCenterX_; // ステージ名の中心X
	int stageNameTextCenterY_ = ringCenterY_; // ステージ名中心Y

	// アニメーション関連

	SelectAnimState animState_ = SelectAnimState::Idle;
	float animTimer_ = 0.0f;
	//float animState_ = 0.5f;
	// 累積回転用
	float ringBaseAngle_ = 0.0f;   // 現在の確定角度
	float ringAnimAngle_ = 0.0f;   // 遷移中の角度

	int rotationDir_ = 1; // 1: 時計回り, -1: 反時計回り

	//ArrowPulse arrowPulseState_ = ArrowPulse::None;
	float arrowAnimTimer_ = 0.0f; // 矢印の上下アニメーション用タイマー
	float arrowPulseTimer_ = 0.0f; // 矢印のパルスアニメーション用タイマー

	float arrowAnimSpeed_ = 0.08f; // 矢印のアニメーション速度

	bool arrowPulseActive_ = false; // 矢印のパルスアニメーションがアクティブかどうか

	// 描画パラメータ
	int explanFontHandle;
	float ringScale_ = 1.4f;
	float ringAngle_ = 0.0f;

	float stageNameAlpha_ = 1.0f;
	float stageNameScale_ = 1.0f;
	float stageNameAngle_ = 0.0f;

	float arrowOffset_ = 0.0f; // 矢印の上下のオフセット
	float arrowScale_ = 1.0f; // 矢印のスケール

	float stageSelectAlpha_;
	float stageSelectScale_;

	int offScreenHandle_;

	int live2DScreenHandle_;

	float fadeAlpha_;

	bool isPause_ = false;
	int pauseCursor_ = 0;
};


