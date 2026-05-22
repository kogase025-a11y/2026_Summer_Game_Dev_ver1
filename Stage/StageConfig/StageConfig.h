#pragma once
#include <memory>
#include <string>
#include <functional>

class SceneGame;
class StageScriptSuper;

struct StageConfig
{
	int stageIndex;
	std::string mapPath;

	std::function<void(SceneGame&)> setUpStage; // ステージ開始時のセットアップ関数
	std::function<std::unique_ptr<StageScriptSuper>()> createScript; // Script生成関数
};

static constexpr int STAGE_MAX = 4;
