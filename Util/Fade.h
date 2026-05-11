#pragma once

// フェード（暗転）を管理・描画するクラス
class Fade
{
public:
	// フェードの状態（待機, In:暗→明, Out:明→暗）
	enum class State { None, In, Out };

	Fade();
	~Fade();

	// フェード開始
	void Start(State state, float duration = 45.0f);
	// 更新処理
	void Update();
	// 描画処理
	void Draw();
	// 真っ黒に塗りつぶす（シーン切り替えの瞬間など）
	void DrawBlack();

	State GetState() const { return state_; }
	bool IsFinished() const { return timer_ >= duration_; }
	float GetProgress() const;

private:
	State state_;
	float timer_;
	float duration_;
};
