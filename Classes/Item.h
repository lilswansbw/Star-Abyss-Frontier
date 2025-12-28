#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"

// 道具类型
enum class ItemType {
  HP,    // 回血
  POWER, // 火力升级
  SKIN   // 皮肤/加分
};

class Item : public cocos2d::Sprite {
public:
  static Item *create(ItemType type);
  static Item *createRandom();
  ItemType getType() const { return _type; }
  void startMove();
  virtual bool init(ItemType type);

private:
  ItemType _type;
};

#endif