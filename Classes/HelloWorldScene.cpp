#include "HelloWorldScene.h"
#include "audio/include/SimpleAudioEngine.h"
#include "MenuScene.h"
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
    _player = nullptr;
    _isPlayerDead = false;

    _player = Sprite::create(playerImageName);

    // 安全检查：防止图片缺失导致崩溃
    if (!_player) {
        // 如果找不到选中的皮肤，就强制用默认的 player_1.png
        _player = Sprite::create("Images/Player/player_1.png");

        // 如果连 player_1 都没有，就用最基础的 CloseNormal.png 保底
        if (!_player) _player = Sprite::create("CloseNormal.png");
    }

    if (_player) {
        _player->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 4 + origin.y));
        _player->setTag(100); // 设置Tag为100，方便以后在触摸移动时获取
        float targetGameWidth = 100.0f;
        float currentWidth = _player->getContentSize().width;
        _player->setScale(targetGameWidth / currentWidth);
        this->addChild(_player, 1); // 层级为1，在背景上面
    }

    // ==========================================
    // 3. 创建触摸监听器 (让飞机跟随手指)
    // ==========================================
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    touchListener->onTouchMoved = [=](Touch* touch, Event* event) {

        if (_isPlayerDead) return;
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

    // 开启我方射击：每 0.2 秒发一发
    this->schedule(schedule_selector(HelloWorld::playerShoot), 0.2f);

    // 开启敌方射击：每 1.5 秒让所有敌人尝试开火一次
    this->schedule(schedule_selector(HelloWorld::enemyShoot), 1.5f);

    return true;
}

void HelloWorld::update(float dt)
{
    // 1. 设置滚动速度 (像素/秒)
    // 之前的 5.0f 是每帧移动，现在改成 300.0f * dt 是每秒移动 300 像素
    // 这样无论帧率高低，速度都是均匀的，会非常丝滑
    this->checkCollisions();
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

void HelloWorld::checkCollisions()
{
    if (_isPlayerDead || !_player) return; // 主角死了就不检测了

    // 获取主角的“碰撞箱” 
    Rect playerRect = _player->getBoundingBox();
    playerRect.origin.x += 10;
    playerRect.size.width -= 20;
    playerRect.origin.y += 10;
    playerRect.size.height -= 20;

    // 遍历所有敌机
    for (auto it = _enemies.begin(); it != _enemies.end(); )
    {
        Enemy* enemy = *it;

        // 获取敌人的碰撞箱
        Rect enemyRect = enemy->getBoundingBox();

        // === [判断相交] ===
        if (playerRect.intersectsRect(enemyRect))
        {
            // 1. 播放爆炸
            spawnExplosion(_player->getPosition()); // 主角炸
            spawnExplosion(enemy->getPosition());   // 敌人炸

            // 2. 移除敌人
            enemy->removeFromParentAndCleanup(true);
            it = _enemies.erase(it); 

            // 3. 主角死亡
            _player->setVisible(false); // 隐藏主角
            _isPlayerDead = true;

            // 4. 触发游戏结束 
            this->scheduleOnce([=](float dt) {
                this->gameOver();
                }, 1.0f, "GameOverDelay");

            // 既然主角死了，就不用检测其他敌人了，直接跳出
            break;
        }
        else
        {
            ++it;
        }
    }
}

void HelloWorld::gameOver()
{
    // 1. 获取屏幕大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 2. 创建一个半透明的黑色遮罩 (Overlay)
    auto layer = LayerColor::create(Color4B(0, 0, 0, 180)); // 黑色，180透明度
    this->addChild(layer, 100); // 层级设得很高，盖住所有东西

    // 3. 显示 "GAME OVER" 大字
    auto label = Label::createWithTTF("GAME OVER", "fonts/Nabla-Regular-VariableFont_EDPT,EHLT.ttf", 64);
    if (!label) label = Label::createWithSystemFont("GAME OVER", "Arial", 64);

    label->setPosition(visibleSize.width / 2, visibleSize.height * 0.7);
    label->setTextColor(Color4B::RED);
    label->enableOutline(Color4B::YELLOW, 2);
    this->addChild(label, 101);

    // 4. 创建 "Return to Menu" (返回菜单) 按钮 
    auto itemLabel = Label::createWithTTF("Back to Menu", "fonts/PermanentMarker-Regular.ttf", 40);
    if (!itemLabel) itemLabel = Label::createWithSystemFont("Back to Menu", "Arial", 40);

    auto menuItem = MenuItemLabel::create(itemLabel, [=](Ref* sender) {
        // 点击后，切换回菜单场景
        // 停止所有正在播放的音效 (爆炸声、枪声等)
        SimpleAudioEngine::getInstance()->stopAllEffects();

        // 2. 停止背景音乐 (战斗BGM)
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();

        auto menuScene = MenuScene::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, menuScene));
        });
    menuItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.4);
    menuItem->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(1.0f, 1.1f), ScaleTo::create(1.0f, 1.0f), nullptr)));

    // 5. 创建菜单容器
    auto menu = Menu::create(menuItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 101);
}

void HelloWorld::spawnExplosion(Vec2 pos)
{
    // 1. 加载整张大图
    auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Effect/Effect.png");

    // 算出每一小帧的宽高 
    int cols = 3;
    int rows = 3;
    float frameWidth = texture->getContentSize().width / cols;
    float frameHeight = texture->getContentSize().height / rows;

    // 2. 创建动画
    auto animation = Animation::create();

    // 从左到右，从上到下切图
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // 计算当前帧在图片里的矩形区域 (Rect)
            auto rect = Rect(c * frameWidth, r * frameHeight, frameWidth, frameHeight);

            // 创建这一帧
            auto frame = SpriteFrame::createWithTexture(texture, rect);
            animation->addSpriteFrame(frame);
        }
    }

    animation->setDelayPerUnit(0.05f); 
    animation->setRestoreOriginalFrame(false);

    // 3. 创建展示用的精灵 (用第一帧初始化)
    auto explosion = Sprite::createWithSpriteFrame(animation->getFrames().front()->getSpriteFrame());
    explosion->setPosition(pos);
    explosion->setScale(2.0f); // 调整大小
    this->addChild(explosion, 10); // 放在最上层

    // 4. 播放并销毁
    auto animate = Animate::create(animation);
    auto remove = RemoveSelf::create();

    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playEffect("Sound/explode.mp3");

    explosion->runAction(Sequence::create(animate, remove, nullptr));
}

void HelloWorld::playerShoot(float dt)
{
    // 如果主角死了，就不能射击
    if (_isPlayerDead || !_player) return;

    // 1. 创建子弹
    auto bullet = Sprite::create("Images/Bullet/bullet_player.png");
    if (!bullet) return;

    // 2. 设置位置 (从飞机头顶发射)
    float startX = _player->getPositionX();
    float startY = _player->getPositionY() + _player->getBoundingBox().size.height / 2;
    bullet->setPosition(startX, startY);
    bullet->setScale(0.8f); // 稍微调小一点

    // 3. 让子弹飞 (向上飞到屏幕外)
    float flyTime = 1.0f; // 飞行时间，越小越快
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float endY = visibleSize.height + 100; // 飞出屏幕上方

    auto move = MoveTo::create(flyTime, Vec2(startX, endY));
    auto remove = CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeBullet, this));

    bullet->runAction(Sequence::create(move, remove, nullptr));

    // 4. 加入容器和场景
    // tag 设为 200，方便以后识别
    bullet->setTag(200);
    this->addChild(bullet, 0); // 层级0，在飞机下面一点点比较自然
    _playerBullets.pushBack(bullet);

    // 5. 播放发射音效
    SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.ogg");
}

void HelloWorld::enemyShoot(float dt)
{
    if (_isPlayerDead) return;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 遍历所有活着的敌人
    for (auto enemy : _enemies) {
        // CCRANDOM_0_1() 返回 0.0 到 1.0 之间的随机数
        if (CCRANDOM_0_1() < 0.8f) {

            // 1. 创建子弹
            auto bullet = Sprite::create("Images/Bullet/bullet_enemy.png");
            if (!bullet) continue;

            // 2. 位置：从敌机底部发射
            float startX = enemy->getPositionX();
            float startY = enemy->getPositionY() - enemy->getBoundingBox().size.height / 2;
            bullet->setPosition(startX, startY);
            bullet->setScale(0.8f);

            // 3. 轨迹：向下飞
            // 这里可以做成“追踪弹”，但先做简单的垂直向下
            float endY = -50; // 屏幕下方
            float flyTime = 1.5f; // 敌机子弹通常慢一点，给玩家躲避时间

            auto move = MoveTo::create(flyTime, Vec2(startX, endY));
            auto remove = CallFuncN::create([=](Node* node) {
                node->removeFromParentAndCleanup(true);
                _enemyBullets.eraseObject(static_cast<Sprite*>(node));
                });

            bullet->runAction(Sequence::create(move, remove, nullptr));

            this->addChild(bullet, 0);
            _enemyBullets.pushBack(bullet);
        }
    }
}

void HelloWorld::removeBullet(Node* bullet)
{
    if (bullet) {
        bullet->removeFromParentAndCleanup(true);
        _playerBullets.eraseObject(static_cast<Sprite*>(bullet));
    }
}