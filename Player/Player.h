#pragma once

#include <memory>
#include <string>
#include "../Manager/Input/InputManager.h"
#include "../Resource/ImageFile.h"
#include "../Resource/SoundFile.h"
#include <DxLib.h>
#include"../Util/Rect.h"

// 前方宣言
class FileManager;
class Stage;
class SceneGame;

/**
 * @brief プレイヤーキャラクターを制御するクラス
 */
class Player {
public:
    // --- 定数 (バランス調整用) ---
    static const int SIZE_X = 96;	            // プレイヤーの横幅
    static const int SIZE_Y = 96;	            // プレイヤーの縦幅
    // 当たり判定のサイズ（トイレットペーパーの「芯」を含めた内側の白い部分に合わせるイメージ）
     // --- 判定サイズ調整用 ---
    static constexpr float HIT_W = 120.0f;      // 横幅（水に当たるのが早すぎたらここを少し減らす）
    static constexpr float HIT_H = 130.0f;      // 縦幅
    static constexpr float OFFSET_Y = 0.0f;    // 地面への沈み込み（アイテム用）
    static constexpr float ANIM_SPEED = 0.1f;   // アニメーション速度
    static constexpr float MAX_MOVE_SPEED = 10.5f; // 最大移動速度
    static constexpr float MOVE_ACC = 0.25f;    // 移動加速度
    static constexpr float MOVE_DEC = 0.05f;    // 移動減速度
    static constexpr float MAX_JUMP_POW = 20.0f; // ジャンプ初速度
    static constexpr float GRAVITY = 0.25f;     // 重力
    static constexpr int   INPUT_JUMP_FRAME = 6; // ジャンプ入力の猶予(大ジャンプ用)

    // アニメーション状態の定義
    enum class ANIM_STATE { IDLE, RUN, JUMP, DAMAGED, MAX };

public:
    // --- コンストラクタ / デストラクタ ---
    Player(Stage* stage, FileManager& fileMng);
    ~Player();

    // --- 初期化・解放 ---
    bool SystemInit(); // システム的な初期化
    void GameInit();   // ゲーム開始時のリセット処理
    bool Release();    // リソース解放

    // --- 更新・描画 ---
    void Update();                          // (旧インターフェース用)
    void Update(const InputManager& input); // メイン更新処理
    void Draw();                            // (旧インターフェース用)
    void Draw(float cameraX, int playerGraphHandle) const; // メイン描画処理

    // --- 外部アクセッサ ---
    const char* GetStateName() const;
    float GetX() const;
    float GetY() const;
    int   GetDirtLevel() const { return dirtLevel_; }
	

    // --- 特殊アクション ---
    void PlayGoalSound();           // ゴールSE再生
    void StartInvincible(float sec); // 無敵状態の開始

    //void AddDirt();                 // 汚れを1段階上げる

   
    // ★これを追加（自分の当たり判定の箱を作る）
      // 当たり判定の四角を返す
    Rect GetHitBox() const {
        Rect r;
        r.w = HIT_W;
        r.h = HIT_H;
        r.x = positionX_ - (r.w / 2.0f);
        r.y = positionY_ - r.h + OFFSET_Y;
        return r;
    }
   

    

    // ★これを追加（ギミックから「汚せ！」と言われた時に実行する）
    void AddDirt() {
        if (!isInvincible_ && dirtLevel_ < 3) {
            dirtLevel_++;
            // もし Player.cpp で puddleSe_ をロードしているなら、ここで鳴らせます
            // PlaySoundMem(puddleSe_->GetHandle(), DX_PLAYTYPE_BACK);
        }
    }

    void SetInPuddle(bool inPuddle) { isInPuddle_ = inPuddle; }

private:
    // --- 内部更新用サブ関数 (Updateから分割) ---
    void ProcessMove(const InputManager& input); // 左右移動入力
    void ProcessJump(const InputManager& input); // ジャンプ入力
    void UpdateInvincibleTimer();                // 無敵タイマー更新
    void UpdateStateName(const InputManager& input); // 状態名の更新
    void CheckTerrainCollision();                // 地形(水たまり等)判定
	 
    //void ApplyDirt(); // 汚れを1段階上げ、無敵なら無視する処理

    // 段差衝突判定の共通処理
    void CheckStepCollision(float sStart, float sEnd, float sTop, float pHalf, float prevX);

    // --- 物理計算系 ---
    void Move();               // 実際の座標計算と壁判定
    void Accele(float speed);  // 加速
    void Decelerate(float spd);// 減速
    void AddGravity();         // 重力加算
    void Jump();               // ジャンプ実行
    void SetJumpPow(float pow);// 上昇速度設定

    

private:
    // 依存ポインタ
    SceneGame* sceneGame_;
    FileManager& fileMng_;
    Stage* stage_;

    // 座標・物理量
    float positionX_ = 300.0f;
    float positionY_ = 760.0f;
    float velocityX_ = 0.0f;
    float velocityY_ = 0.0f;
    float angle_ = 0.0f; // 転がる回転角

    // フラグ・タイマー類
    bool onGround_ = true;
    bool isInPuddle_ = false; // 水たまり中か
    bool wasInPuddle_ = false; // 前フレーム水たまりか
    bool isInDirty_ = false; // 汚れ地面中か
    bool wasInDirty_ = false; // 前フレーム汚れ地面か
    int  dirtLevel_ = 0;     // 汚れ段階(0~3)
    int  jumpTimer_ = 0;     // ジャンプ継続タイマー

    float invincibleTimer_ = 0.0f; // 無敵残り時間
    bool  isInvincible_ = false; // 無敵フラグ

    // リソースハンドル
    std::shared_ptr<ImageFile> particleTex;
    std::shared_ptr<ImageFile> wetTexs[3];
    std::shared_ptr<SoundFile> jumpSe_;
    std::shared_ptr<SoundFile> fallSe_;
    std::shared_ptr<SoundFile> damageSe_;
    std::shared_ptr<SoundFile> goalSe_;
    std::shared_ptr<SoundFile> puddleSe_;

    const char* stateName_ = "Idle";
};