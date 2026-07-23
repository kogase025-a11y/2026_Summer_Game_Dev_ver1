#include "GimmickRain.h"
#include "RainManager.h"
#include "../Scene/SceneGame.h"

GimmickRain::GimmickRain(float x, float y, int w, int h, SceneGame* scene)
    : worldX_(x), worldY_(y), width_(w), height_(h), scene_(scene)
{
    rainManager_ = std::make_unique<RainManager>(200);
}

GimmickRain::~GimmickRain() {}

void GimmickRain::Update(float deltaTime) {
    if (rainManager_) {
        rainManager_->Update(deltaTime, worldX_, worldY_, width_, height_, scene_);
    }
}

void GimmickRain::Draw(int scrollX, int scrollY) const {
    if (rainManager_) {
        rainManager_->Draw(scrollX, scrollY);
    }
}

void GimmickRain::OnTouch(Player& player, float deltaTime) {

    if(player.GetHitBox().IsHit(GetHitBox())) {
        // ƒvƒŒƒCƒ„[‚ª‰J‚Ì“–‚½‚è”»’è‚ÉG‚ê‚½ê‡‚Ìˆ—
        scene_->OnPlayerHitRain();
	}
}

// yRectƒGƒ‰[‚ğ‰ğÁIz
Rect GimmickRain::GetHitBox() const {
    Rect r;
    r.x = 0.0f;
    r.y = 0.0f;
    r.w = static_cast<float>(width_);
    r.h = static_cast<float>(height_);
    return r;
}