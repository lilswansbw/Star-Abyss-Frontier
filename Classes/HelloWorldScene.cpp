#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// 初始化函数：游戏开始时执行一次
bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    //初始界面
    //创建主角飞机
    auto player = Sprite::create("Images/player/myplane.png"); 
    if (player) {
        player->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 4 + origin.y));
        player->setTag(100); // 给它贴个标签叫100，方便以后找
        this->addChild(player, 1);
    }

    // 2. 创建触摸监听器 (让飞机跟随手指)
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    touchListener->onTouchMoved = [=](Touch* touch, Event* event) {
        // 获取主角
        auto target = this->getChildByTag(100);
        if (target) {
            // 让飞机跟着手指移动
            target->setPosition(target->getPosition() + touch->getDelta());
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    //创建第一张背景
    _bg1 = Sprite::create("Images/Background/bg.jpg");
    _bg1->setAnchorPoint(Vec2::ZERO);
    _bg1->setPosition(0, 0);

    // 算出要把图片放大多少倍才能填满屏幕宽度
    float scaleX = visibleSize.width / _bg1->getContentSize().width;
    float scaleY = visibleSize.height / _bg1->getContentSize().height;
    // 取宽和高中比较大的那个比例，保证填满屏幕（可能会裁剪掉一点边）
    float scale = std::max(scaleX, scaleY);

    _bg1->setScale(scale); // 执行放大
    this->addChild(_bg1, -1);

    //创建第二张背景
    _bg2 = Sprite::create("Images/Background/bg.jpg");
    _bg2->setAnchorPoint(Vec2::ZERO);
    _bg2->setScale(scale); // 第二张也要放大同样的倍数

    // 因为图变大了，第二张图必须放在第一张图“放大后”的头顶上
    // getBoundingBox().size.height 获取的是放大后的高度
    _bg2->setPosition(0, _bg1->getBoundingBox().size.height);

    this->addChild(_bg2, -1);

    this->schedule(schedule_selector(HelloWorld::createEnemy), 1.2f);

    return true;
}

//创建单个敌机随机位置+向下移动
void HelloWorld::createEnemy(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //创建敌机
    auto enemy = Sprite::create("Images/Enemy/eplane.png");
    if (!enemy) {
        enemy->setScale(2.0f);
    }

    //随机设置敌机初始X坐标,但要在屏幕宽度范围内，避免超出边界
    float randomX = origin.x + CCRANDOM_0_1() * (visibleSize.width - enemy->getContentSize().width * enemy->getScaleX());
    //初始Y坐标：在屏幕上方外侧，可以避免突然出现
    float startY = origin.y + visibleSize.height + enemy->getContentSize().height * enemy->getScaleY();
    enemy->setPosition(Vec2(randomX, startY));

    //随机设置敌机移动速度2-4秒从顶部飞到屏幕底部
    float moveTime = 2.0f + CCRANDOM_0_1() * 2.0f;
    //移动目标，屏幕下方外侧超出后移除
    float endY = origin.y - enemy->getContentSize().height * enemy->getScaleY();
    auto moveAction = MoveTo::create(moveTime, Vec2(randomX, endY));

    //敌机飞出屏幕后自动移除避免内存泄漏
    auto removeAction = CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeEnemy, this));
    auto sequence = Sequence::create(moveAction, removeAction, nullptr);
    enemy->runAction(sequence);

    //添加敌机到场景和管理容器
    this->addChild(enemy, 0); // 层级0，在背景之上、主角之下
    _enemies.pushBack(enemy);
}

//移除敌机，从场景和容器中同时删除
void HelloWorld::removeEnemy(cocos2d::Node* enemy)
{
    if (enemy) {
        // 从场景中移除
        enemy->removeFromParentAndCleanup(true);
        // 从容器中移除
        _enemies.eraseObject(static_cast<Sprite*>(enemy));
    }
}

