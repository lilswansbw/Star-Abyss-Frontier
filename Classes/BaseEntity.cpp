#include "BaseEntity.h"
#include "cocos2d.h"

bool BaseEntity::init() {
    if (!Sprite::init()) return false;
    _hp = 1;
    _speed = 0;
    _isAlive = true;
   // _maxHP = _hp;
    
    /*初始化血条背景
    _hpBarBg = cocos2d::Sprite::create("Images/Effect/hp_bg.png");
    _hpBarBg->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
    _hpBarBg->setPosition(0, this->getContentSize().height / 2 + 10);//位于实体上方
    _hpBarBg->setScaleX(0.8f);//缩放适配
    this->addChild(_hpBarBg, 10);

    //初始化血条前景
    _hpBar = cocos2d::Sprite::create("Images/Effect/hp_.png");
    _hpBar->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
    _hpBar->setPosition(_hpBarBg->getPosition());
    _hpBar->setScaleX(0.8f);
    this->addChild(_hpBar, 11);*/
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
    /*更新血条显示, 按比例缩小
    float scale = (float)_hp / _maxHP;
    _hpBar->setScaleX(scale * 0.8f); //保持宽度比例*/

    if (_hp <= 0) {
        onDeath();
    }
}