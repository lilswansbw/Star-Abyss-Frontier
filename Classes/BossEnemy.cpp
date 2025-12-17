#include "BossEnemy.h"

BossEnemy* BossEnemy::create(const std::string& imgPath, int hp) {
    BossEnemy* enemy = new BossEnemy();
    if (enemy && enemy->init(imgPath, hp)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

bool BossEnemy::init(const std::string& imgPath, int hp) {
    if (!Enemy::init(imgPath, hp)) return false;
    this->setScale(2.0f); //体积放大2倍
    return true;
}

void BossEnemy::startMove(float duration, float endY) {
    Enemy::startMove(duration * 1.1, endY);
}// 移动速度减慢