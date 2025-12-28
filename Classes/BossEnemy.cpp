#include "BossEnemy.h"

BossEnemy *BossEnemy::create(const std::string &imgPath, int hp) {
  BossEnemy *enemy = new BossEnemy();
  if (enemy && enemy->init(imgPath, hp)) {
    enemy->autorelease();
    return enemy;
  }
  CC_SAFE_DELETE(enemy);
  return nullptr;
}

bool BossEnemy::init(const std::string &imgPath, int hp) {
  if (!Enemy::init(imgPath, hp))
    return false;
  this->setScale(2.0f);
  this->setupHPBar("Images/Effect/hp_bg.png", "Images/Effect/hp_.png", 0.4f);
  return true;
}

void BossEnemy::startMove(float duration, float endY) {
  float currentX = this->getPositionX();

  // 入场动画
  auto moveEnter = MoveTo::create(duration, Vec2(currentX, endY));

  // 入场后开始巡逻
  auto startPatrol = CallFunc::create([this]() { this->moveRandomly(); });

  auto seq = Sequence::create(moveEnter, startPatrol, nullptr);
  this->runAction(seq);
}

void BossEnemy::moveRandomly() {
  auto visibleSize = Director::getInstance()->getVisibleSize();

  // 定义 Boss 活动范围
  float margin = 50.0f;
  float minX = margin;
  float maxX = visibleSize.width - margin;
  float minY = visibleSize.height * 0.5f;
  float maxY = visibleSize.height * 0.9f;

  // 随机目标点
  float randomX = minX + CCRANDOM_0_1() * (maxX - minX);
  float randomY = minY + CCRANDOM_0_1() * (maxY - minY);
  Vec2 targetPos = Vec2(randomX, randomY);

  // 计算移动时间
  float distance = this->getPosition().distance(targetPos);
  float speed = 100.0f;
  float duration = distance / speed;

  // 移动 -> 停顿 -> 下一次移动
  auto move = MoveTo::create(duration, targetPos);
  auto easeMove = EaseSineInOut::create(move);

  float waitTime = 0.5f + CCRANDOM_0_1() * 1.0f;
  auto delay = DelayTime::create(waitTime);

  auto nextMove = CallFunc::create([this]() { this->moveRandomly(); });

  auto seq = Sequence::create(easeMove, delay, nextMove, nullptr);
  this->runAction(seq);
}