#include "StageScriptSuper.h"

class Stage2Script : public StageScriptSuper
{
public:
	void OnStart(SceneGame& scene) override;
	void Update(SceneGame& scene, float deltaTime) override;

private:
	enum class Step
	{
		Start,
		LButton,
		//Absorb,
		//PlayerAbsorbBefore,
		//PlayerAbsorbAfter,
		Finished
	};

	Step step_ = Step::Start;
	int startX_ = 0;
};
