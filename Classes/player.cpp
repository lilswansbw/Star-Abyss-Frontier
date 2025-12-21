#include "Player.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

bool Player::init() {
    // 1. 初始化父类
    if (!BaseEntity::init()) return false;

    // ==========================================
    // [逻辑搬运] 读取皮肤 + 创建图片
    // ==========================================
    this->startShoot();
    int skinId = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
    std::string imgName = StringUtils::format("Images/Player/player_%d.png", skinId);

    // 给自己(this)设置贴图，而不是创建新的 Sprite
    this->initWithFile(imgName);

    // 如果没图的保底逻辑
    if (this->getContentSize().width == 0) {
        this->initWithFile("Images/Player/player_1.png");
    }

    // 设置初始属性
    this->setHP(5);
    this->setupHPBar("Images/Effect/hp_bg.png", "Images/Effect/hp_.png", 0.15f);
    this->setTag(100);
    /*_maxHP = 5;*/
    // 统一缩放逻辑
    float targetGameWidth = 100.0f;
    float currentWidth = this->getContentSize().width;
    if (currentWidth > 0) {
        this->setScale(targetGameWidth / currentWidth);
    }

    // ==========================================
    // [逻辑搬运] 开启触摸 (自给自足)
    // ==========================================
    this->initTouchLogic();

    return true;
}

void Player::startShoot() {
    // 【修改】去掉最后的 "shoot_key"，只保留前4个参数
    // 注意：Cocos2d-x 4.0 建议用 CC_REPEAT_FOREVER 代替 kRepeatForever
    this->schedule(CC_SCHEDULE_SELECTOR(Player::autoShootLogic), 0.2f, CC_REPEAT_FOREVER, 0.0f);
}

void Player::stopShoot() {
    // 【修改】取消时，也要用选择器，不能用字符串
    this->unschedule(CC_SCHEDULE_SELECTOR(Player::autoShootLogic));
}

void Player::initTouchLogic() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true); // 吞噬触摸，防止穿透

    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };

    // [逻辑搬运] 移动与边界限制
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (!this->isAlive()) return;

        auto winSize = Director::getInstance()->getWinSize();
        // 注意：这里直接用 this，因为代码就在飞机类里
        Vec2 newPos = this->getPosition() + touch->getDelta();

        // 边界计算
        float halfW = this->getBoundingBox().size.width / 2;
        float halfH = this->getBoundingBox().size.height / 2;

        // 限制 X
        if (newPos.x < halfW) newPos.x = halfW;
        if (newPos.x > winSize.width - halfW) newPos.x = winSize.width - halfW;

        // 限制 Y
        if (newPos.y < halfH) newPos.y = halfH;
        if (newPos.y > winSize.height - halfH) newPos.y = winSize.height - halfH;

        this->setPosition(newPos);
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Player::autoShootLogic(float dt) {
    // 只有活着才能射击
    if (!this->isAlive() || _hp <= 0) {
        return;
    }

    // 调用你原本写的 shoot() 生成子弹
    // 注意：如果你原来的 shoot() 只负责创建 Sprite 并返回，
    // 你可能需要在这里把返回的子弹加到父节点 (Scene) 里，
    // 或者你原来的 shoot() 已经把自己加进去了。

    // 假设你原来的 shoot() 只是创建并返回 Sprite：
    auto bullet = this->shoot();
    if (bullet) {
        // 如果 bullet 还没加到场景，记得加一下，或者由 shoot 内部处理
        if (!bullet->getParent()) {
            this->getParent()->addChild(bullet);
        }
        // 如果你需要把子弹加到 Scene 的数组里管理，这里可能需要回调或者事件分发
        // 但为了简单，先确保能射出来且能停下
    }
}

void Player::onDeath() {
    // 1. 【核按钮】停止所有定时器（包括射击、移动等一切update）
    // 这比 unschedule("name") 或 stopShoot() 更彻底、更安全
    this->unscheduleAllCallbacks();

    // 2. 停止所有动作（移动、动画）
    this->stopAllActions();

    // 3. 确保数据层面已经死了
    _hp = 0;
    _isAlive = false;

    // 4. 视觉消失
    this->setVisible(false);

    // 5. 播放音效等...
}

cocos2d::Sprite* Player::shoot() {
    SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.mp3");

    auto bullet = Sprite::create("Images/Bullet/bullet_player.png");
    if (bullet) {
        float startX = this->getPositionX();
        float startY = this->getPositionY() + this->getBoundingBox().size.height / 2;
        bullet->setPosition(startX, startY);
        bullet->setScale(0.8f);

        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto move = MoveTo::create(1.0f, Vec2(startX, visibleSize.height + 200));
        bullet->runAction(move);
    }
    return bullet;
}