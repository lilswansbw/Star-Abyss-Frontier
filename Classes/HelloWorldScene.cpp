#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// 初始化函数：游戏开始时执行一次
bool HelloWorld::init()
{
    // 1. 父类初始化检查
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ==========================================
    // 2. 创建主角飞机
    // ==========================================

    // 从存档中读取皮肤 ID
    int skinId = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
    // 拼凑图片文件名
    std::string playerImageName = StringUtils::format("Images/Player/player_%d.png", skinId);

    auto player = Sprite::create(playerImageName);

    // 安全检查：防止图片缺失导致崩溃
    if (!player) {
        // 如果找不到选中的皮肤，就强制用默认的 player_1.png
        player = Sprite::create("Images/Player/player_1.png");

        // 如果连 player_1 都没有，就用最基础的 CloseNormal.png 保底
        if (!player) player = Sprite::create("CloseNormal.png");
    }

    if (player) {
        player->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 4 + origin.y));
        player->setTag(100); // 设置Tag为100，方便以后在触摸移动时获取
        this->addChild(player, 1); // 层级为1，在背景上面
    }

    // ==========================================
    // 3. 创建触摸监听器 (让飞机跟随手指)
    // ==========================================
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    touchListener->onTouchMoved = [=](Touch* touch, Event* event) {
        // 获取主角飞机
        auto target = this->getChildByTag(100);
        if (target) {
            // 让飞机位置跟随手指移动的增量
            target->setPosition(target->getPosition() + touch->getDelta());
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // ==========================================
    // 4. 创建滚动背景
    // ==========================================

    // 创建第一张背景
    _bg1 = Sprite::create("Images/Background/bg.jpg");
    _bg1->setAnchorPoint(Vec2::ZERO);
    _bg1->setPosition(0, 0);

    // 计算缩放比例，确保填满屏幕宽度和高度
    float scaleX = visibleSize.width / _bg1->getContentSize().width;
    float scaleY = visibleSize.height / _bg1->getContentSize().height;
    // 取宽高中较大的比例，保证完全覆盖屏幕不留黑边
    float scale = std::max(scaleX, scaleY);

    _bg1->setScale(scale);
    this->addChild(_bg1, -1); // 层级 -1，最底层

    // 创建第二张背景（用于轮替滚动）
    _bg2 = Sprite::create("Images/Background/bg.jpg");
    _bg2->setAnchorPoint(Vec2::ZERO);
    _bg2->setScale(scale); // 保持同样的缩放

    // 关键位置：放在第一张图放大后的头顶上
    // getBoundingBox().size.height 获取的是缩放后的实际高度
    _bg2->setPosition(0, _bg1->getBoundingBox().size.height);

    this->addChild(_bg2, -1);

    // ==========================================
    // 5. 开启定时器
    // ==========================================
    // 每隔 1.2秒 调用一次 createEnemy 生成敌人
    this->schedule(schedule_selector(HelloWorld::createEnemy), 1.2f);

    // 别忘了开启 update 来处理背景滚动（如果你的 update 函数里写了逻辑的话）
    this->scheduleUpdate();

    return true;
}

// 定时器回调：随机生成敌机 + 移动逻辑
void HelloWorld::createEnemy(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建敌机精灵
    auto enemy = Sprite::create("Images/Enemy/eplane.png");

    if (enemy) {
        enemy->setScale(2.0f); // 如果图片太小，放大一点

        // 随机X坐标：确保在屏幕宽度范围内，不超出边界
        float randomX = origin.x + CCRANDOM_0_1() * (visibleSize.width - enemy->getContentSize().width * enemy->getScaleX());

        // 初始Y坐标：在屏幕上方一点点，这样看起来是从屏幕外飞进来的
        float startY = origin.y + visibleSize.height + enemy->getContentSize().height * enemy->getScaleY();
        enemy->setPosition(Vec2(randomX, startY));

        // 移动逻辑：随机 2-4 秒飞到底部
        float moveTime = 2.0f + CCRANDOM_0_1() * 2.0f;

        // 目标Y坐标：屏幕下方看不见的地方
        float endY = origin.y - enemy->getContentSize().height * enemy->getScaleY();
        auto moveAction = MoveTo::create(moveTime, Vec2(randomX, endY));

        // 动作序列：先移动，移动结束后执行移除函数（防止内存泄漏）
        auto removeAction = CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeEnemy, this));
        auto sequence = Sequence::create(moveAction, removeAction, nullptr);

        enemy->runAction(sequence);

        // 添加到场景和容器
        this->addChild(enemy, 0);
        _enemies.pushBack(enemy); // 放入 Vector 容器管理（用于之后的碰撞检测）
    }
}

// 移除敌机：从场景和容器中同时删除
void HelloWorld::removeEnemy(cocos2d::Node* enemy)
{
    if (enemy) {
        // 从屏幕上移除
        enemy->removeFromParentAndCleanup(true);

        // 从管理容器中移除 (注意类型转换)
        _enemies.eraseObject(static_cast<Sprite*>(enemy));
    }
}