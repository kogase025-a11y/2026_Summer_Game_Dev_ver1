#include <DxLib.h>
#include "Application/Application.h"

//----------------
// WinMain関数
//----------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//アプリケーションクラスの実体化したインスタンスの作成
	Application application;

	//初期化処理
	if (application.SystemInit() == false)return -1;

	//ゲーム起動
	application.Run();

	//解放処理
	if (!application.Release()) return -1;

	return 0;		//ゲーム終了
}
