#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"

// 定义道具类型枚举
enum class ItemType {
    HP,     // 回血药水
    POWER,  // 火力升级
    SKIN    // 皮肤碎片 (或者作为高分奖励)
};

class Item : public cocos2d::Sprite {
public:
    // 静态工厂方法：指定类型创建
    static Item* create(ItemType type);

    // 静态工厂方法：随机创建一个道具 (方便在掉落时调用)
    static Item* createRandom();

    // 获取当前道具类型
    ItemType getType() const { return _type; }

    // 开始移动 (飘落逻辑)
    void startMove();

    // 初始化
    virtual bool init(ItemType type);

private:
    ItemType _type;
};

#endif