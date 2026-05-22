#pragma once
class SceneGame;

class StageScriptSuper
{
public:
	virtual ~StageScriptSuper() = default;
	virtual void Update(SceneGame&, float) {}
	virtual void OnStart(SceneGame& scene) {}
};
