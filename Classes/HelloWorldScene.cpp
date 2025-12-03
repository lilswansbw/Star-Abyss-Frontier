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
    // 1. 创建主角飞机 (确保 Resources 文件夹里有一张叫 player.png 的图，如果没有，先用 CloseNormal.png 代替测试)
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

    // 1. 创建第一张背景
    _bg1 = Sprite::create("Images/Background/bg.jpg");
    _bg1->setAnchorPoint(Vec2::ZERO);
    _bg1->setPosition(0, 0);

    // === 新增：计算缩放比例 ===
    // 算出要把图片放大多少倍才能填满屏幕宽度
    float scaleX = visibleSize.width / _bg1->getContentSize().width;
    float scaleY = visibleSize.height / _bg1->getContentSize().height;
    // 取宽和高中比较大的那个比例，保证填满屏幕（可能会裁剪掉一点边）
    float scale = std::max(scaleX, scaleY);

    _bg1->setScale(scale); // 执行放大
    this->addChild(_bg1, -1);

    // 2. 创建第二张背景
    _bg2 = Sprite::create("Images/Background/bg.jpg");
    _bg2->setAnchorPoint(Vec2::ZERO);
    _bg2->setScale(scale); // 第二张也要放大同样的倍数

    // === 关键修改：位置也要变 ===
    // 因为图变大了，第二张图必须放在第一张图“放大后”的头顶上
    // getBoundingBox().size.height 获取的是放大后的高度
    _bg2->setPosition(0, _bg1->getBoundingBox().size.height);

    this->addChild(_bg2, -1);


    return true;
}