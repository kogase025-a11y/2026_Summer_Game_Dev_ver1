#pragma once
#include "StageScriptSuper.h"

class Stage3Script : public StageScriptSuper
{
public:
	void OnStart(SceneGame& scene) override;
	void Update(SceneGame& scene, float deltaTime) override;

private:
	enum class Step
	{
		Start,
		PlayerAbsorbBefore,
		PlayerAbsorbAfter,
		Finished
	};

	Step step_ = Step::Start;
	int startX_ = 0;
};