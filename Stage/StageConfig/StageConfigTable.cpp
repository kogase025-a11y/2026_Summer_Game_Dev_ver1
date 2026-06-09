#include "StageConfigTable.h"
#include "../StageScript/Stage1Script.h"
#include "../StageScript/Stage2Script.h"
#include "../StageScript/Stage3Script.h"
#include "../../Scene/SceneGame.h"
#include "../Stage.h"


//const std::vector<StageConfig>& GetStageConfigs()
//{
//	static std::vector<StageConfig> configs = {
//		{
//			1,
//			"Resource/Map/json/stage_01.json",
//			[](SceneGame& scene)
//			{
//
//			},
//			[]()
//			{
//				return std::make_unique<Stage1Script>();
//			}
//		},
//		{
//			2,
//			"Resource/Map/json/stage_02.json",
//			[](SceneGame& scene)
//			{
//
//
//			},
//			[]()
//			{
//				return std::make_unique<Stage2Script>();
//			}
//		},
//		{
//			3,
//			"Resource/Map/json/stage_03.json",
//			[](SceneGame& scene)
//			{
//
//			},
//			[]()
//			{
//				return std::make_unique<Stage3Script>();
//			}
//		},
//
//	};
//}
//const void CreateLightButton(const std::vector<std::tuple<int, int, int, int, int>>& wallParams, SceneGame& scene, int worldX, int worldY)
//{
//	
//}
//
//
