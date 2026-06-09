#pragma once
#include <vector>
#include <tuple>
#include "StageConfig.h"

class SceneGame;
class LightWallGimmick;

const std::vector<StageConfig>& GetStageConfigs();

const void CreateLightButton(const std::vector<std::tuple<int, int, int, int, int>>& wallParams, SceneGame& scene, int x, int y);



