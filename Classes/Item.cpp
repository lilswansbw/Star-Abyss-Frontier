#include "Item.h"

USING_NS_CC;

Item* Item::create(ItemType type) {
    Item* item = new Item();
    if (item && item->init(type)) {
        item->autorelease();
        return item;
    }
    CC_SAFE_DELETE(item);
    return nullptr;
}

Item* Item::createRandom() {
    // 简单的随机逻辑：33%概率
    float r = CCRANDOM_0_1();
    if (r < 0.25f) {
        return create(ItemType::HP);
    }
    // 0.25 ~ 0.85 (60%): 掉火力升级 (这才是玩家想要的爽感！)
    else if (r < 0.85f) {
        return create(ItemType::POWER);
    }
    // 剩下的 (15%): 掉皮肤/加分
    else {
        return create(ItemType::SKIN);
    }
}

bool Item::init(ItemType type) {
    _type = type;
    std::string imgPath;

    switch (type) {
        case ItemType::HP:    imgPath = "Images/Item/item_hp.png"; break;
        case ItemType::POWER: imgPath = "Images/Item/item_power.png"; break;
        case ItemType::SKIN:  imgPath = "Images/Item/item_skin.png"; break;
        default: return false;
    }

    if (!Sprite::initWithFile(imgPath)) return false;

    // 1. 基础大小 (保持 0.5，如果你觉得还是整体偏大，可以改 0.4)
    float baseScale = 0.2f;
    this->setScale(baseScale);

    // ==========================================
    // 【修改核心】减小呼吸幅度
    // ==========================================
    // 之前是 +/- 0.1 (幅度太大)
    // 现在改为 +/- 0.03 (非常轻微的浮动，只为了提示它是活的)
    float delta = 0.03f;

    auto scaleBig = ScaleTo::create(0.6f, baseScale + delta); // 时间稍微拉长到 0.6s 更柔和
    auto scaleSmall = ScaleTo::create(0.6f, baseScale - delta);

    this->runAction(RepeatForever::create(Sequence::create(scaleBig, scaleSmall, nullptr)));

    return true;
}

void Item::startMove() {
    // 简单的向下飘落逻辑
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 移动到底部之下
    float duration = 4.0f; // 飘慢一点，给玩家反应时间
    auto move = MoveTo::create(duration, Vec2(this->getPositionX(), -100));

    // 飞出屏幕后自我销毁
    auto remove = RemoveSelf::create();

    this->runAction(Sequence::create(move, remove, nullptr));
}   