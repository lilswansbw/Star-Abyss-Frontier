#include "HelloWorldScene.h"
#include "Player.h"  // 确保你已经新建了 Player 类
#include "Enemy.h"   // 确保你已经新建了 Enemy 类
#include "MenuScene.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

// ============================================================
// 1. 【修复 LNK2019】补上 createScene 的实现
//    这是导致 MenuScene 报错的元凶
// ============================================================
Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// ============================================================
// 2. 初始化函数
// ============================================================
bool HelloWorld::init()
{
    if (!Scene::init()) return false;

    // 播放背景音乐
    SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/game_bgm.mp3", true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ----------------------------------------------------
    // [重构部分] 创建主角 (现在代码非常简洁)
    // ----------------------------------------------------
    // 注意：如果 Player::create() 报错，请检查 Player.h 是否正确引入
    _player = Player::create();
    if (_player) {
        // 设置初始位置
        _player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 4));
        this->addChild(_player, 1);
    }

    // ----------------------------------------------------
    // [原有逻辑] 创建背景
    // ----------------------------------------------------
    _bg1 = Sprite::create("Images/Background/bg.jpg");
    // 如果没有图片，加个安全判断防止崩溃
    if (_bg1) {
        _bg1->setAnchorPoint(Vec2::ZERO);
        _bg1->setPosition(0, 0);

        // 简单的背景适配逻辑
        float scaleX = visibleSize.width / _bg1->getContentSize().width;
        float scaleY = visibleSize.height / _bg1->getContentSize().height;
        float scale = std::max(scaleX, scaleY);
        _bg1->setScale(scale);
        this->addChild(_bg1, -2);

        _bg2 = Sprite::create("Images/Background/bg.jpg");
        _bg2->setAnchorPoint(Vec2::ZERO);
        _bg2->setScale(scale);
        _bg2->setFlippedY(true); // 镜像翻转，做无缝连接
        _bg2->setPosition(0, _bg1->getBoundingBox().size.height);
        this->addChild(_bg2, -2);
    }

    // 创建星空背景 (装饰)
    _stars1 = Sprite::create("Images/Background/stars.png");
    if (_stars1) {
        _stars1->setAnchorPoint(Vec2::ZERO);
        _stars1->setPosition(0, 0);
        _stars1->setBlendFunc(BlendFunc::ADDITIVE);
        this->addChild(_stars1, -1);

        _stars2 = Sprite::create("Images/Background/stars.png");
        _stars2->setAnchorPoint(Vec2::ZERO);
        _stars2->setPosition(0, _stars1->getContentSize().height);
        _stars2->setBlendFunc(BlendFunc::ADDITIVE);
        this->addChild(_stars2, -1);
    }

    // ----------------------------------------------------
    // [原有逻辑] 开启定时器
    // ----------------------------------------------------
    this->schedule(schedule_selector(HelloWorld::createEnemy), 1.2f);

    // 【关键】开启 update 调度，否则 update 函数不会被调用
    this->scheduleUpdate();

    this->schedule(schedule_selector(HelloWorld::playerShoot), 0.2f);
    this->schedule(schedule_selector(HelloWorld::enemyShoot), 1.5f);

    // 监听主角死亡事件 (从 Player 发出的)
    auto deadListener = EventListenerCustom::create("PLAYER_DEAD_EVENT", [=](EventCustom* event) {
        this->gameOver();
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(deadListener, this);

    return true;
}

// ============================================================
// 3. 【修复 LNK2001】补上 update 的实现
//    这是导致 HelloWorldScene.obj 报错的元凶
// ============================================================
void HelloWorld::update(float dt)
{
    // 每一帧都检查碰撞
    this->checkCollisions();

    // 背景滚动逻辑
    float scrollSpeed = 200.0f;
    float moveAmount = scrollSpeed * dt;

    if (_bg1 && _bg2) {
        _bg1->setPositionY(_bg1->getPositionY() - moveAmount);
        _bg2->setPositionY(_bg2->getPositionY() - moveAmount);

        float bgHeight = _bg1->getBoundingBox().size.height;

        if (_bg1->getPositionY() <= -bgHeight) {
            _bg1->setPositionY(_bg2->getPositionY() + bgHeight);
        }
        if (_bg2->getPositionY() <= -bgHeight) {
            _bg2->setPositionY(_bg1->getPositionY() + bgHeight);
        }
    }

    // 星空滚动
    float starSpeed = 400.0f * dt;
    if (_stars1 && _stars2) {
        _stars1->setPositionY(_stars1->getPositionY() - starSpeed);
        _stars2->setPositionY(_stars2->getPositionY() - starSpeed);
        float h = _stars1->getBoundingBox().size.height;
        if (_stars1->getPositionY() <= -h) _stars1->setPositionY(_stars2->getPositionY() + h);
        if (_stars2->getPositionY() <= -h) _stars2->setPositionY(_stars1->getPositionY() + h);
    }
}

// ============================================================
// 4. 生成敌人 (使用新的 Enemy 封装逻辑)
// ============================================================
void HelloWorld::createEnemy(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建敌机 (假设 Enemy 类已经重构好)
    auto enemy = Enemy::create("Images/Enemy/eplane.png", 3);
    if (!enemy) return;

    // 随机位置
    float enemyWidth = enemy->getContentSize().width * enemy->getScaleX();
    float minX = origin.x + enemyWidth / 2;
    float maxX = origin.x + visibleSize.width - enemyWidth / 2;
    float randomX = minX + CCRANDOM_0_1() * (maxX - minX);

    float startY = origin.y + visibleSize.height + enemy->getContentSize().height;
    enemy->setPosition(Vec2(randomX, startY));

    this->addChild(enemy, 0);
    _enemies.pushBack(enemy);

    // 让敌机开始移动 (调用 Enemy 的接口)
    float moveTime = 2.0f + CCRANDOM_0_1() * 2.0f;
    float endY = origin.y - 100;

    // 如果 Enemy 类里没有 startMove，请确保你也更新了 Enemy.h/.cpp
    enemy->startMove(moveTime, endY);
}

// ============================================================
// 5. 碰撞检测
// ============================================================
void HelloWorld::checkCollisions()
{
    // 如果主角死了或者还没创建，跳过
    if (_isPlayerDead || !_player) return;

    // 强制转换检查状态
    Player* p = dynamic_cast<Player*>(_player);
    if (p && !p->isAlive()) return;

    // 遍历敌机
    for (auto e_it = _enemies.begin(); e_it != _enemies.end(); )
    {
        Enemy* enemy = *e_it;
        bool enemyDead = false;

        // 5.1 子弹 vs 敌机
        for (auto b_it = _playerBullets.begin(); b_it != _playerBullets.end(); )
        {
            Sprite* bullet = *b_it;
            if (bullet->getBoundingBox().intersectsRect(enemy->getBoundingBox()))
            {
                bullet->removeFromParentAndCleanup(true);
                b_it = _playerBullets.erase(b_it); // 安全删除子弹

                enemy->hurt(); // 敌机受伤

                if (!enemy->isAlive()) {
                    enemyDead = true;
                    break; // 敌机死，跳出子弹循环
                }
            }
            else {
                ++b_it;
            }
        }

        if (enemyDead) {
            e_it = _enemies.erase(e_it); // 从列表移除敌机
            continue;
        }

        // 5.2 主角 vs 敌机
        // 使用 inset 缩小碰撞箱，手感更好
        Rect playerRect = _player->getBoundingBox();
        playerRect.origin.x += 15; playerRect.size.width -= 30;
        playerRect.origin.y += 15; playerRect.size.height -= 30;

        if (playerRect.intersectsRect(enemy->getBoundingBox()))
        {
            spawnExplosion(_player->getPosition());
            enemy->hurt();

            // 主角死亡逻辑
            if (p) p->onDeath();
            _isPlayerDead = true;

            // 1秒后游戏结束
            this->scheduleOnce([=](float dt) { this->gameOver(); }, 1.0f, "GameOverDelay");

            e_it = _enemies.erase(e_it);
            break;
        }
        else {
            ++e_it;
        }
    }
}

// ============================================================
// 6. 射击与子弹管理
// ============================================================
void HelloWorld::playerShoot(float dt)
{
    Player* p = dynamic_cast<Player*>(_player);
    if (!p || !p->isAlive()) return;

    // 调用 Player 的射击接口，获取子弹
    Sprite* bullet = p->shoot();

    if (bullet) {
        bullet->setTag(200);
        this->addChild(bullet, 0);
        _playerBullets.pushBack(bullet);
    }
}

void HelloWorld::enemyShoot(float dt)
{
    if (_isPlayerDead) return;

    for (auto enemy : _enemies) {
        if (CCRANDOM_0_1() < 0.4f) { // 降低点概率
            auto bullet = Sprite::create("Images/Bullet/bullet_enemy.png");
            if (!bullet) continue;

            float startX = enemy->getPositionX();
            float startY = enemy->getPositionY() - enemy->getBoundingBox().size.height / 2;
            bullet->setPosition(startX, startY);
            bullet->setScale(0.8f);

            float endY = -50;
            float flyTime = 1.5f;

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

// ============================================================
// 7. 辅助函数 (GameOver, Explosion)
// ============================================================
void HelloWorld::spawnExplosion(Vec2 pos)
{
    // 这里使用你之前改好的切图逻辑
    auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Effect/Effect.png");
    if (!texture) return;

    int cols = 3;
    int rows = 3;
    float frameWidth = texture->getContentSize().width / cols;
    float frameHeight = texture->getContentSize().height / rows;

    auto animation = Animation::create();
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            auto rect = Rect(c * frameWidth, r * frameHeight, frameWidth, frameHeight);
            auto frame = SpriteFrame::createWithTexture(texture, rect);
            animation->addSpriteFrame(frame);
        }
    }
    animation->setDelayPerUnit(0.05f);
    animation->setRestoreOriginalFrame(false);

    auto explosion = Sprite::createWithSpriteFrame(animation->getFrames().front()->getSpriteFrame());
    explosion->setPosition(pos);
    explosion->setScale(2.0f);
    this->addChild(explosion, 10);

    auto animate = Animate::create(animation);
    auto remove = RemoveSelf::create();
    SimpleAudioEngine::getInstance()->playEffect("Sound/explode.mp3");
    explosion->runAction(Sequence::create(animate, remove, nullptr));
}

void HelloWorld::gameOver()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto layer = LayerColor::create(Color4B(0, 0, 0, 180));
    this->addChild(layer, 100);

    auto label = Label::createWithSystemFont("GAME OVER", "Arial", 64);
    label->setPosition(visibleSize.width / 2, visibleSize.height * 0.7);
    label->setTextColor(Color4B::RED);
    this->addChild(label, 101);

    auto itemLabel = Label::createWithSystemFont("Back to Menu", "Arial", 40);
    auto menuItem = MenuItemLabel::create(itemLabel, [=](Ref* sender) {
        SimpleAudioEngine::getInstance()->stopAllEffects();
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        auto menuScene = MenuScene::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, menuScene));
        });
    menuItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.4);

    auto menu = Menu::create(menuItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 101);
}