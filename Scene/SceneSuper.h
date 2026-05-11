#pragma once

class SceneSuper
{
public:
	enum class SceneID
	{
		NONE,
		TITLE,
		STAGE_SELECT,
		GAME,
		CLEAR,
		EXIT,
	};

	virtual ~SceneSuper() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsEnd() const { return isEnd_; }
	bool GetIsTransition() const { return isTransition_; }
	void SetIsTransition(bool isTransition) { isTransition_ = isTransition; }
	SceneID GetNextScene() const { return nextScene_; }

	virtual void SetTransitionIn(float t) { transitionIn_ = t; }
	virtual void SetTransitionOut(float t) { transitionOut_ = t; }

protected:
	void EndScene(SceneID next)
	{
		isEnd_ = true;
		nextScene_ = next;
	}

private:
	bool isEnd_ = false;
	bool isTransition_ = false;
	SceneID nextScene_ = SceneID::NONE;
	float transitionIn_ = 0.0f;
	float transitionOut_ = 0.0f;
};
