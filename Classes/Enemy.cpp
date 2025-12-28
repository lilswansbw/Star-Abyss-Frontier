#include "Enemy.h"
#include "audio/include/SimpleAudioEngine.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace CocosDenshion;

Enemy *Enemy::create(const std::string &imgPath, int hp) {
  Enemy *enemy = new Enemy();
  if (enemy && enemy->init(imgPath, hp)) {
    enemy->autorelease();
    return enemy;
  }
  CC_SAFE_DELETE(enemy);
  return nullptr;
}

bool Enemy::init(const std::string &imgPath, int hp) {
  if (!Sprite::initWithFile(imgPath)) {
    return false;
  }
  _hp = hp;
  _isAlive = true;
  return true;
}

void Enemy::startMove(float duration, float endY) {
  float currentX = this->getPositionX();

  // 移动动作
  auto moveAction =
      cocos2d::MoveTo::create(duration, cocos2d::Vec2(currentX, endY));

  // 出界移除
  auto removeWhenOut = cocos2d::CallFuncN::create([this](cocos2d::Node *node) {
    if (this->_isAlive) {
      this->removeFromParentAndCleanup(true);
      this->_isAlive = false;
    }
  });

  this->runAction(
      cocos2d::Sequence::create(moveAction, removeWhenOut, nullptr));
}

void Enemy::hurt() {
  if (!this->isAlive())
    return;

  this->takeDamage(1);

  // 受伤闪烁
  if (this->isAlive()) {
    auto tintRed = TintTo::create(0.1f, 255, 0, 0);
    auto tintBack = TintTo::create(0.1f, 255, 255, 255);
    this->runAction(Sequence::create(tintRed, tintBack, nullptr));
  }

  if (!this->isAlive()) {
    boom();
  }
}

void Enemy::boom() {
  this->stopAllActions();

  // 加载爆炸资源
  auto texture = Director::getInstance()->getTextureCache()->addImage(
      "Images/Effect/Effect.png");
  if (!texture) {
    this->removeFromParentAndCleanup(true);
    return;
  }

  // 准备动画帧
  int cols = 3;
  int rows = 3;
  float frameWidth = texture->getContentSize().width / cols;
  float frameHeight = texture->getContentSize().height / rows;

  auto animation = Animation::create();
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      auto rect =
          Rect(c * frameWidth, r * frameHeight, frameWidth, frameHeight);
      auto frame = SpriteFrame::createWithTexture(texture, rect);
      animation->addSpriteFrame(frame);
    }
  }
  animation->setDelayPerUnit(0.05f);
  animation->setRestoreOriginalFrame(false);

  // 统一爆炸大小和方向
  this->setScale(1.2f);
  this->setFlippedY(false);
  this->setColor(Color3B::WHITE);

  // 播放动画并移除
  auto animate = Animate::create(animation);
  auto removeSelf = CallFuncN::create([this](Node *node) {
    node->removeFromParentAndCleanup(true);
    this->_isAlive = false;
  });

  this->runAction(Sequence::create(animate, removeSelf, nullptr));
}