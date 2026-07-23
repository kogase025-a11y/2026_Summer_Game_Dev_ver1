#include "RainManager.h"
#include "DxLib.h"
#include "../Scene/SceneGame.h"
#include "../Util/Vector2.h"
#include "../Util/Rect.h"

RainManager::RainManager(int maxDrops) {
    drops_.resize(maxDrops);
}

void RainManager::Update(float deltaTime, float areaX, float areaY, int areaW, int areaH, SceneGame* scene) {
    timer_ += deltaTime;
    if (timer_ > 0.02f) {
        for (auto& d : drops_) {
            if (!d.isActive) {
                d.isActive = true;
                d.x = areaX + static_cast<float>(rand() % (areaW > 0 ? areaW : 1));
                d.y = areaY;
                d.speed = 400.0f + static_cast<float>(rand() % 200);
                break;
            }
        }
        timer_ = 0.0f;
    }

    for (auto& d : drops_) {
        if (!d.isActive) continue;

        d.y += d.speed * deltaTime;
        d.x -= 30.0f * deltaTime;

        // y‰®ª”»’èzSceneGame‚É’Ç‰Á‚µ‚½ŠÖ”‚ðŒÄ‚Ô
        if (scene->CheckStageCollision(d.x, d.y)) {
            d.isActive = false;
        }

        if (d.y > areaY + static_cast<float>(areaH)) d.isActive = false;
    }
}

void RainManager::Draw(int scrollX, int scrollY) const {
    for (const auto& d : drops_) {
        if (!d.isActive) continue;
        int sx = static_cast<int>(d.x) - scrollX;
        int sy = static_cast<int>(d.y) - scrollY;
        DrawLine(sx, sy, sx - 1, sy + 8, GetColor(150, 150, 255));
    }
}