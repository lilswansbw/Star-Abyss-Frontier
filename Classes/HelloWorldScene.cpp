#include "HelloWorldScene.h"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
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

    SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/game_bgm.mp3", true);

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
        float targetGameWidth = 100.0f;
        float currentWidth = player->getContentSize().width;
        player->setScale(targetGameWidth / currentWidth);
        this->addChild(player, 1); // 层级为1，在背景上面
    }

    // ==========================================
    // 3. 创建触摸监听器 (让飞机跟随手指)
    // ==========================================
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    touchListener->onTouchMoved = [=](Touch* touch, Event* event) {
        // 1. 获取主角飞机
        auto target = this->getChildByTag(100);

        if (target) {
            // 2. 获取屏幕大小
            auto winSize = Director::getInstance()->getWinSize();

            // 3. 获取飞机当前的宽高的一半 (用于计算边缘)
            // getBoundingBox() 会自动算上缩放比例，非常方便
            float halfWidth = target->getBoundingBox().size.width / 2;
            float halfHeight = target->getBoundingBox().size.height / 2;

            // 4. 计算原本想去的新位置
            Vec2 newPos = target->getPosition() + touch->getDelta();

            // 5. 【核心逻辑】限制 X 轴 (左右不飞出去)
            // 如果小于左边界，就等于左边界
            if (newPos.x < halfWidth) {
                newPos.x = halfWidth;
            }
            // 如果大于右边界，就等于右边界
            if (newPos.x > winSize.width - halfWidth) {
                newPos.x = winSize.width - halfWidth;
            }

            // 6. 【核心逻辑】限制 Y 轴 (上下不飞出去)
            // 下边界
            if (newPos.y < halfHeight) {
                newPos.y = halfHeight;
            }
            // 上边界
            if (newPos.y > winSize.height - halfHeight) {
                newPos.y = winSize.height - halfHeight;
            }

            // 7. 最后才赋值修正后的安全位置
            target->setPosition(newPos);
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
    this->addChild(_bg1, -2); // 层级 -1，最底层

    // 创建第二张背景（用于轮替滚动）
    _bg2 = Sprite::create("Images/Background/bg.jpg");
    _bg2->setAnchorPoint(Vec2::ZERO);
    _bg2->setScale(scale); // 保持同样的缩放
    _bg2->setFlippedY(true);
    // 关键位置：放在第一张图放大后的头顶上
    // getBoundingBox().size.height 获取的是缩放后的实际高度
    _bg2->setPosition(0, _bg1->getBoundingBox().size.height);

    this->addChild(_bg2, -2);

    _stars1 = Sprite::create("Images/Background/stars.png");
    _stars1->setAnchorPoint(Vec2::ZERO);
    _stars1->setPosition(0, 0);
    _stars1->setBlendFunc(BlendFunc::ADDITIVE);
    this->addChild(_stars1, -1);

    _stars2 = Sprite::create("Images/Background/stars.png");
    _stars2->setAnchorPoint(Vec2::ZERO);
    _stars2->setPosition(0, _stars1->getContentSize().height);
    _stars2->setBlendFunc(BlendFunc::ADDITIVE);
    this->addChild(_stars2, -1);

    // ==========================================
    // 5. 开启定时器
    // ==========================================
    // 每隔 1.2秒 调用一次 createEnemy 生成敌人
    this->schedule(schedule_selector(HelloWorld::createEnemy), 1.2f);

    // 别忘了开启 update 来处理背景滚动（如果你的 update 函数里写了逻辑的话）
    this->scheduleUpdate();

    return true;
}

void HelloWorld::update(float dt)
{
    // 1. 设置滚动速度 (像素/秒)
    // 之前的 5.0f 是每帧移动，现在改成 300.0f * dt 是每秒移动 300 像素
    // 这样无论帧率高低，速度都是均匀的，会非常丝滑
    float scrollSpeed = 200.0f;
    float moveAmount = scrollSpeed * dt;

    if (_bg1 && _bg2) {
        // 2. 移动两张背景
        _bg1->setPositionY(_bg1->getPositionY() - moveAmount);
        _bg2->setPositionY(_bg2->getPositionY() - moveAmount);

        // 3. 获取背景高度
        float bgHeight = _bg1->getBoundingBox().size.height;

        // 4. 循环逻辑（关键修改点！）
        // 【修改】必须是 <= -bgHeight，表示整张图都跑出去了
        if (_bg1->getPositionY() <= -bgHeight) {
            // 【关键】接到另一张图的确切位置上方，消除缝隙
            _bg1->setPositionY(_bg2->getPositionY() + bgHeight);
        }

        if (_bg2->getPositionY() <= -bgHeight) {
            _bg2->setPositionY(_bg1->getPositionY() + bgHeight);
        }
    }

    float starSpeed = 400.0f * dt; // 快 (是星云的4倍)
    if (_stars1 && _stars2) {
        _stars1->setPositionY(_stars1->getPositionY() - starSpeed);
        _stars2->setPositionY(_stars2->getPositionY() - starSpeed);

        float h = _stars1->getBoundingBox().size.height;
        if (_stars1->getPositionY() <= -h) _stars1->setPositionY(_stars2->getPositionY() + h);
        if (_stars2->getPositionY() <= -h) _stars2->setPositionY(_stars1->getPositionY() + h);
    }
}
// 定时器回调：随机生成敌机 + 移动逻辑
void HelloWorld::createEnemy(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建敌机精灵
    auto enemy = Enemy::create("Images/Enemy/eplane.png", 1);



    // 随机X坐标：确保在屏幕宽度范围内，不超出边界
    float enemyWidth = enemy->getContentSize().width * enemy->getScaleX();
    float enemyHeight = enemy->getContentSize().height * enemy->getScaleY();

    // 随机X：保证敌机完全在屏幕内（左右无溢出）
    float minX = origin.x + enemyWidth / 2;
    float maxX = origin.x + visibleSize.width - enemyWidth / 2;
    float randomX = minX + CCRANDOM_0_1() * (maxX - minX);

    // 初始Y：屏幕上方外侧（避免突然出现）
    float startY = origin.y + visibleSize.height + enemyHeight;
    enemy->setPosition(Vec2(randomX, startY));

    // 移动逻辑：随机 2-4 秒飞到底部
    float moveTime = 2.0f + CCRANDOM_0_1() * 2.0f;

    // 目标Y坐标：屏幕下方看不见的地方
    float endY = origin.y - enemy->getContentSize().height * enemy->getScaleY();
    auto moveAction = MoveTo::create(moveTime, Vec2(randomX, endY));

    auto removeWhenOut = CallFuncN::create([this](Node* node) {
        Enemy* enemy = static_cast<Enemy*>(node);
        if (enemy->isAlive()) {
            enemy->removeFromParentAndCleanup(true);
            _enemies.eraseObject(enemy);
        }
        });
    enemy->runAction(Sequence::create(moveAction, removeWhenOut, nullptr));
    // 添加到场景和容器
    this->addChild(enemy, 0);
    _enemies.pushBack(enemy); // 放入 Vector 容器管理（用于之后的碰撞检测）

}

// 移除敌机：从场景和容器中同时删除
void HelloWorld::removeEnemy(cocos2d::Node* enemy)
{
    if (enemy) {
        // 从屏幕上移除
        enemy->removeFromParentAndCleanup(true);

        // 从管理容器中移除
        _enemies.eraseObject(static_cast<Enemy*>(enemy));
    }
}