#pragma once
#include "StageScriptSuper.h"

class Stage1Script : public StageScriptSuper
{
public:
    void OnStart(SceneGame& scene) override;
    void Update(SceneGame& scene, float deltaTime) override;
private:
    enum class Step
    {
        Start,
        TouchedLight,
        WentRight,
        NearStorage,
        TouchStorage,
        LeftStorage,
        TouchedStorageAgain,
        CatalystBehavior,// 触媒の挙動を見せるステップ ここではなくゲーム全体で管理するべきかもしれない
        Clear,
        Finished,
    };

    Step step_ = Step::Start;
    int startX_ = 0;
    bool nowTouchingStorage_ = false;
    bool wasTouchingStorage_ = false;
};

