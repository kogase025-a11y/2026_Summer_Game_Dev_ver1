#include "SceneClear.h"

#include "../Manager/FileManager.h"

SceneClear::SceneClear(FileManager&)
{
}

void SceneClear::Update()
{
	// ENTERでタイトルへ戻る
	if (CheckHitKey(KEY_INPUT_RETURN) != 0)
	{
		EndScene(SceneID::TITLE);
	}
}

void SceneClear::Draw()
{
	// クリア表示
	DrawString(20, 110, "CLEAR: PRESS ENTER", GetColor(0, 255, 0));
}
