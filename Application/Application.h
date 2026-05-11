#pragma once
#include <memory>

class SceneManager;
class FileManager;

class Application
{
public:
	static constexpr int SCREEN_SIZE_WID =	1920;		//ゲーム画面の横サイズ
	static constexpr int SCREEN_SIZE_HIG = 1080;		//ゲーム画面縦サイズ


	Application();								//コンストラクタ
	~Application();								//デストラクタ

	bool SystemInit(void);						//初期化処理
	void Run(void);								//ゲーム起動
	void Update(void);							//更新処理
	void Draw(void);							//描画処理
	bool Release(void);							//解放処理


private:
	std::unique_ptr<SceneManager> sceneMana;	//シーンマネージャーのインスタンスのポインタ
	std::unique_ptr<FileManager> fileMng_;
};

