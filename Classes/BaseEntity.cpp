#include "BaseEntity.h"

bool BaseEntity::init() {
    if (!Sprite::init()) return false;
    _hp = 1;
    _speed = 0;
    return true;
}

void BaseEntity::takeDamage(int damage) {
    if (_hp > 0) {
        _hp -= damage;
        if (_hp <= 0) {
            _hp = 0;
            onDeath(); // 触发死亡逻辑 (由子类 Enemy 或 Player 具体实现)
        }
    }
}