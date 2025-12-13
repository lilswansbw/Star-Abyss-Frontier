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
    this->setTag(100);

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

cocos2d::Sprite* Player::shoot() {
    SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.mp3");

    auto bullet = Sprite::create("Images/Bullet/bullet_player.png");
    if (bullet) {
        float startX = this->getPositionX();
        float startY = this->getPositionY() + this->getBoundingBox().size.height / 2;
        bullet->setPosition(startX, startY);
        bullet->setScale(0.8f);

        // [修改] 只需要让他一直往上飞就行了
        // 飞出屏幕后的销毁工作，交给 HelloWorldScene::checkCollisions 去做
        float flyTime = 1.0f;
        auto visibleSize = Director::getInstance()->getVisibleSize();
        // 飞得远一点，确保肯定能飞出去
        auto move = MoveTo::create(flyTime, Vec2(startX, visibleSize.height + 200));

        bullet->runAction(move);
    }
    return bullet;
}

void Player::onDeath() {
    // 隐藏自己，播放爆炸 (可选)
    this->setVisible(false);
}