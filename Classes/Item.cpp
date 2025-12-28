#include "Item.h"

USING_NS_CC;

Item *Item::create(ItemType type) {
  Item *item = new Item();
  if (item && item->init(type)) {
    item->autorelease();
    return item;
  }
  CC_SAFE_DELETE(item);
  return nullptr;
}

Item *Item::createRandom() {
  // 25% 回血，60% 火力，15% 皮肤
  float r = CCRANDOM_0_1();
  if (r < 0.25f) {
    return create(ItemType::HP);
  } else if (r < 0.85f) {
    return create(ItemType::POWER);
  } else {
    return create(ItemType::SKIN);
  }
}

bool Item::init(ItemType type) {
  _type = type;
  std::string imgPath;

  switch (type) {
  case ItemType::HP:
    imgPath = "Images/Item/item_hp.png";
    break;
  case ItemType::POWER:
    imgPath = "Images/Item/item_power.png";
    break;
  case ItemType::SKIN:
    imgPath = "Images/Item/item_skin.png";
    break;
  default:
    return false;
  }

  if (!Sprite::initWithFile(imgPath))
    return false;

  // 缩放与呼吸动画
  float baseScale = 0.2f;
  this->setScale(baseScale);

  float delta = 0.03f;
  auto scaleBig = ScaleTo::create(0.6f, baseScale + delta);
  auto scaleSmall = ScaleTo::create(0.6f, baseScale - delta);
  this->runAction(
      RepeatForever::create(Sequence::create(scaleBig, scaleSmall, nullptr)));

  return true;
}

void Item::startMove() {
  auto visibleSize = Director::getInstance()->getVisibleSize();

  // 向下飘落
  float duration = 4.0f;
  auto move = MoveTo::create(duration, Vec2(this->getPositionX(), -100));
  auto remove = RemoveSelf::create();
  this->runAction(Sequence::create(move, remove, nullptr));
}