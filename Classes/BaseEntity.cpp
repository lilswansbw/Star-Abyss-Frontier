#include "BaseEntity.h"
#include "cocos2d.h"


USING_NS_CC;

bool BaseEntity::init() {
  if (!Sprite::init())
    return false;
  _hp = 1;
  _speed = 0;
  _isAlive = true;
  _maxHp = 1;
  _hpBar = nullptr;
  _hpBarBg = nullptr;
  return true;
}

void BaseEntity::setupHPBar(const std::string &bgImg,
                            const std::string &fillImg, float scale) {
  _maxHp = _hp;
  _originalScale = scale;

  // 创建背景
  _hpBarBg = Sprite::create(bgImg);
  if (_hpBarBg) {
    _hpBarBg->setPosition(this->getContentSize().width / 2,
                          this->getContentSize().height + 15);
    _hpBarBg->setScaleX(scale);
    _hpBarBg->setScaleY(scale);
    _hpBarBg->setOpacity(180);
    this->addChild(_hpBarBg, 10);
  }

  // 创建前景
  _hpBar = Sprite::create(fillImg);
  if (_hpBar) {
    _hpBar->setAnchorPoint(Vec2(0, 0.5f));

    // 计算宽度比例
    float widthRatio = 1.0f;
    if (_hpBarBg) {
      float bgWidth = _hpBarBg->getContentSize().width;
      float barWidth = _hpBar->getContentSize().width;
      widthRatio = bgWidth / barWidth;
    }

    // 设置位置
    if (_hpBarBg) {
      float bgRealWidth =
          _hpBarBg->getContentSize().width * _hpBarBg->getScaleX();
      float startX = _hpBarBg->getPositionX() - (bgRealWidth / 2);
      _hpBar->setPosition(startX, _hpBarBg->getPositionY());
    }

    _hpBar->setScaleX(scale * widthRatio);
    _hpBar->setScaleY(scale);
    _hpBar->setOpacity(200);
    this->addChild(_hpBar, 11);
  }
}

void BaseEntity::updateHPBar() {
  if (_hpBar && _maxHp > 0) {
    float percent = (float)_hp / (float)_maxHp;
    if (percent < 0)
      percent = 0;
    if (percent > 1)
      percent = 1;

    float widthRatio = 1.0f;
    if (_hpBarBg) {
      widthRatio =
          _hpBarBg->getContentSize().width / _hpBar->getContentSize().width;
    }
    _hpBar->setScaleX(_originalScale * widthRatio * percent);
  }
}

void BaseEntity::takeDamage(int damage) {
  if (_hp > 0) {
    _hp -= damage;
    updateHPBar();

    // 受伤闪烁效果
    this->stopActionByTag(110);
    auto flash =
        Sequence::create(TintTo::create(0.1f, Color3B::RED),
                         TintTo::create(0.1f, Color3B::WHITE), nullptr);
    flash->setTag(110);
    this->runAction(flash);

    if (_hp <= 0) {
      _hp = 0;
      _isAlive = false;
      this->setColor(Color3B::WHITE);
      onDeath();
    }
  }
}

void BaseEntity::heal(int amount) {
  if (!_isAlive)
    return;
  _hp += amount;
  if (_hp > _maxHp) {
    _hp = _maxHp;
  }
  updateHPBar();

  // 治疗闪烁效果
  auto tintGreen = TintTo::create(0.1f, 0, 255, 0);
  auto tintBack = TintTo::create(0.1f, 255, 255, 255);
  this->runAction(Sequence::create(tintGreen, tintBack, nullptr));
}