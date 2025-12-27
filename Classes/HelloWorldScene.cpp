#include "HelloWorldScene.h"
#include "BossEnemy.h"
#include "Enemy.h"
#include "GameOverScene.h"
#include "Item.h"
#include "MenuScene.h"
#include "Player.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

// ============================================================
// 1. 【修复 LNK2019】补上 createScene 的实现
//    这是导致 MenuScene 报错的元凶
// ============================================================
Scene *HelloWorld::createScene() { return HelloWorld::create(); }

// ============================================================
// 2. 初始化函数
// ============================================================
bool HelloWorld::init() {
  if (!Scene::init())
    return false;

  // 播放背景音乐
  SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/game_bgm.mp3",
                                                        true);

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
  this->schedule(schedule_selector(HelloWorld::enemyShoot), 0.5f);

  // 监听主角死亡事件 (从 Player 发出的)
  auto deadListener = EventListenerCustom::create(
      "PLAYER_DEAD_EVENT", [=](EventCustom *event) { this->gameOver(); });
  _eventDispatcher->addEventListenerWithSceneGraphPriority(deadListener, this);

  _currentBoss = nullptr;
  // 添加Boss生成调度，初步设定每30秒生成一次
  this->schedule(schedule_selector(HelloWorld::createBoss), 10.0f);

  SimpleAudioEngine::getInstance()->setEffectsVolume(0.3f);

  // ==========================================
  // [子弹时间] 添加键盘监听器
  // ==========================================
  _isShiftPressed = false; // 初始化 Shift 键状态

  auto keyboardListener = EventListenerKeyboard::create();

  // 键盘按下事件
  keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode,
                                          Event *event) {
    if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
      _isShiftPressed = true;
    }
  };

  // 键盘释放事件
  keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode,
                                           Event *event) {
    if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
      _isShiftPressed = false;
    }
  };

  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener,
                                                           this);

  _score = 0;
  _scoreLabel = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 24);
  if (_scoreLabel) {
    // 3. 设置位置：放在屏幕左上角
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // X: 左边缘 + 20像素, Y: 顶边缘 - 20像素
    _scoreLabel->setPosition(
        Vec2(origin.x + 60, origin.y + visibleSize.height - 30));

    // 设置颜色 (比如黄色更显眼)
    _scoreLabel->setColor(Color3B::YELLOW);

    // 加到场景里，层级设高一点(100)，保证不被飞机遮住
    this->addChild(_scoreLabel, 100);
  }

  return true;
}

// ============================================================
// 3. 【修复 LNK2001】补上 update 的实现
//    这是导致 HelloWorldScene.obj 报错的元凶
// ============================================================
void HelloWorld::update(float dt) {
  // ==========================================
  // [Time Warp] 子弹时间：动态修改全局时间流速
  // ==========================================
  if (_isShiftPressed) {
    // 进入慢动作模式
    Director::getInstance()->getScheduler()->setTimeScale(0.3f);
    // TODO: 可选视觉反馈 - 让背景变暗
    // this->setColor(Color3B(100, 100, 100));
  } else {
    // 恢复正常速度
    Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    // TODO: 恢复背景颜色
    // this->setColor(Color3B::WHITE);
  }

  // 每一帧都检查碰撞
  this->checkCollisions();
  this->checkItemCollisions();
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
    if (_stars1->getPositionY() <= -h)
      _stars1->setPositionY(_stars2->getPositionY() + h);
    if (_stars2->getPositionY() <= -h)
      _stars2->setPositionY(_stars1->getPositionY() + h);
  }
}

// ============================================================
// 4. 生成敌人 (使用新的 Enemy 封装逻辑)
// ============================================================
// HelloWorldScene.cpp -> createEnemy (威力加强版)

// HelloWorldScene.cpp -> createEnemy (精准修正版)

void HelloWorld::createEnemy(float dt) {
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  Enemy *enemy = nullptr;
  bool isElite = false;
  bool needFlip = false;

  // 【新增】用来标记是不是那个看着特别小的旧飞机
  bool isLegacyPlane = false;

  // 1. 决定类型
  if (CCRANDOM_0_1() < 0.2f) {
    // 精英组
    isElite = true;
    std::string imgPath = (CCRANDOM_0_1() < 0.5f)
                              ? "Images/Enemy/enemy_red.png"
                              : "Images/Enemy/enemy_green.png";
    enemy = Enemy::create(imgPath, 10);
    enemy->setTag(2);
    needFlip = true;
  } else {
    // 普通组
    std::string imgPath;
    if (CCRANDOM_0_1() < 0.5f) {
      imgPath = "Images/Enemy/enemy_white.png";
      needFlip = true;
    } else {
      imgPath = "Images/Enemy/eplane.png";
      needFlip = false;
      isLegacyPlane = true; // <--- 标记它！
    }
    enemy = Enemy::create(imgPath, 3);
    enemy->setTag(1);
  }

  if (!enemy)
    return;

  // 2. 翻转
  if (needFlip) {
    enemy->setFlippedY(true);
  }

  // 3. 标准化缩放 (大小控制)
  // 依然使用 120 (或者你觉得合适的大小)
  const float STANDARD_ENEMY_WIDTH = 120.0f;

  float actualWidth = enemy->getContentSize().width;
  float baseScale = STANDARD_ENEMY_WIDTH / actualWidth;

  if (isElite) {
    enemy->setScale(baseScale * 1.1f);
  } else if (isLegacyPlane) {
    // =================================================
    // 【核心修改】如果是 eplane，额外放大 1.5 倍！
    // =================================================
    enemy->setScale(baseScale * 1.5f);
  } else {
    enemy->setScale(baseScale * 0.95f);
  }

  // 4. 位置与移动 (保持不变)
  float scaledEnemyWidth = enemy->getContentSize().width * enemy->getScaleX();
  float minX = origin.x + scaledEnemyWidth / 2;
  float maxX = origin.x + visibleSize.width - scaledEnemyWidth / 2;
  float h = enemy->getContentSize().height * enemy->getScaleY();
  float startY = origin.y + visibleSize.height + h / 2;

  Vec2 enemyPos;
  for (int i = 0; i < 5; i++) {
    float randomX = minX + CCRANDOM_0_1() * (maxX - minX);
    enemyPos = Vec2(randomX, startY);
    if (!isPosOverlapWithBoss(enemyPos, scaledEnemyWidth))
      break;
  }
  enemy->setPosition(enemyPos);
  this->addChild(enemy, 0);
  _enemies.pushBack(enemy);

  float baseTime = isElite ? 3.5f : 2.0f;
  float moveTime = baseTime + CCRANDOM_0_1() * 1.0f;
  float endY = origin.y - h;
  enemy->startMove(moveTime, endY);
}

// ============================================================
// 5. 碰撞检测
// ============================================================
void HelloWorld::checkCollisions() {
  if (_isPlayerDead || !_player)
    return;

  Player *player = dynamic_cast<Player *>(_player);
  if (!player || !player->isAlive())
    return;

  auto visibleSize = Director::getInstance()->getVisibleSize();

  // =================================================================
  // 1. [敌方子弹] 撞 [主角]
  // =================================================================
  for (auto eb_it = _enemyBullets.begin(); eb_it != _enemyBullets.end();) {
    Sprite *bullet = *eb_it;

    // 越界清理
    if (bullet->getPositionY() < -50) {
      bullet->removeFromParent();
      eb_it = _enemyBullets.erase(eb_it);
      continue;
    }

    // 判定碰撞
    bool hitPlayer = false;
    Rect playerRect = player->getBoundingBox();
    playerRect.origin.x += 15;
    playerRect.size.width -= 30;
    playerRect.origin.y += 15;
    playerRect.size.height -= 30;

    if (bullet->getBoundingBox().intersectsRect(playerRect)) {
      hitPlayer = true;
      bullet->removeFromParentAndCleanup(true);

      player->takeDamage(1);

      if (!player->isAlive()) {
        spawnExplosion(player->getPosition());
        player->onDeath();
        _isPlayerDead = true;
        this->scheduleOnce([=](float dt) { this->gameOver(); }, 1.0f,
                           "GameOverDelay");
      }
    }

    if (hitPlayer) {
      eb_it = _enemyBullets.erase(eb_it);
      if (_isPlayerDead)
        return;
    } else {
      ++eb_it;
    }
  }

  // =================================================================
  // 2. [我方子弹] 撞 [敌机] (改为：遍历子弹 -> 遍历敌人)
  //    这样能保证一颗子弹只命中一个敌人，并且立即消失，避免穿透
  // =================================================================
  for (auto b_it = _playerBullets.begin(); b_it != _playerBullets.end();) {
    Sprite *bullet = *b_it;

    // 越界清理
    if (bullet->getPositionY() > visibleSize.height) {
      bullet->removeFromParent();
      b_it = _playerBullets.erase(b_it);
      continue;
    }

    bool bulletHit = false;

    // 内层循环遍历所有敌人
    for (auto e_it = _enemies.begin(); e_it != _enemies.end();) {
      Enemy *enemy = dynamic_cast<Enemy *>(*e_it);

      // 忽略还要屏幕外的敌人(刚生成)
      if (!enemy || enemy->getPositionY() > visibleSize.height) {
        ++e_it;
        continue;
      }

      if (bullet->getBoundingBox().intersectsRect(enemy->getBoundingBox())) {
        // 命中！
        bulletHit = true;

        // 伤害计算
        int damage = 1;
        if (player)
          damage = player->getDamage();
        enemy->takeDamage(damage);

        // 敌人死亡判定
        if (!enemy->isAlive()) {
          enemy->boom();
          // 计分
          if (dynamic_cast<BossEnemy *>(enemy)) {
            addScore(1000);
            showFloatingScore(enemy->getPosition(), 1000);
          } else {
            addScore(100);
            showFloatingScore(enemy->getPosition(), 100);
          }

          // 掉落道具
          if (CCRANDOM_0_1() < 0.5f) {
            auto item = Item::createRandom();
            item->setPosition(enemy->getPosition());
            this->addChild(item, 5);
            item->startMove();
            _items.pushBack(item);
          }

          // 从数组删除 (boom() 会处理节点移除和爆炸动画)
          e_it = _enemies.erase(e_it);
        } else {
          // 敌人没死，继续检查下一个敌人 (理论上这步不会执行，因为我们要 break)
          // 但如果想做穿透弹，就不 break
          ++e_it;
        }

        // 【关键】子弹命中后，Break 出内层循环
        // 这样这颗子弹就不会再去判定其他敌人了
        break;
      } else {
        ++e_it;
      }
    }

    if (bulletHit) {
      // 子弹没了
      bullet->removeFromParentAndCleanup(true);
      b_it = _playerBullets.erase(b_it);
    } else {
      ++b_it;
    }
  }

  // =================================================================
  // 3. [敌机] 撞 [主角] (身体碰撞)
  // =================================================================
  for (auto e_it = _enemies.begin(); e_it != _enemies.end();) {
    Enemy *enemy = dynamic_cast<Enemy *>(*e_it);
    bool isCrash = false;

    // 既然上面可能已经把死人移除了，这里生还的都是活人
    if (enemy && enemy->getPositionY() <= visibleSize.height) {
      Rect playerRect = player->getBoundingBox();
      playerRect.origin.x += 15;
      playerRect.size.width -= 30;
      playerRect.origin.y += 15;
      playerRect.size.height -= 30;

      if (playerRect.intersectsRect(enemy->getBoundingBox())) {
        isCrash = true;

        spawnExplosion(player->getPosition());

        // 双方都死
        enemy->takeDamage(9999);
        if (!enemy->isAlive())
          enemy->boom();

        player->onDeath();
        _isPlayerDead = true;
        this->scheduleOnce([=](float dt) { this->gameOver(); }, 1.0f,
                           "GameOverDelay");
      }
    }

    if (isCrash) {
      e_it = _enemies.erase(e_it);
      if (_isPlayerDead)
        return;
    } else {
      ++e_it;
    }
  }
}

// ============================================================
// 6. 射击与子弹管理
// ============================================================
void HelloWorld::playerShoot(float dt) {
  Player *p = dynamic_cast<Player *>(_player);
  if (!p || !p->isAlive())
    return;

  // 调用 Player 的射击接口，获取子弹
  Sprite *bullet = p->shoot();

  if (bullet) {
    bullet->setTag(200);
    this->addChild(bullet, 0);
    _playerBullets.pushBack(bullet);
  }
}

void HelloWorld::enemyShoot(float dt) {
  if (_isPlayerDead)
    return;

  for (auto enemy : _enemies) {

    // 射击频率控制
    if (CCRANDOM_0_1() > 0.4f)
      continue;

    // 获取位置
    float startX = enemy->getPositionX();
    float startY =
        enemy->getPositionY() - enemy->getBoundingBox().size.height / 2;

    // ==========================================
    // 情况 BOSS：如果是 Boss -> 发射螺旋弹幕
    // ==========================================
    BossEnemy *boss = dynamic_cast<BossEnemy *>(enemy);
    if (boss) {
      // Boss 螺旋弹幕参数
      const int bulletCount = 12;       // 子弹数量（环绕一周）
      const float bulletSpeed = 200.0f; // 子弹速度
      const float travelTime = 6.0f; // 子弹飞行时间（增加到 6 秒确保飞出屏幕）

      // 【修复】Boss 弹幕从中心发射，而不是从底部
      float bossX = boss->getPositionX();
      float bossY = boss->getPositionY(); // 使用中心位置

      // 静态变量记录旋转偏移，每次射击后递增
      static float rotationOffset = 0.0f;

      // 使用三角函数 (Trigonometry) 计算每颗子弹的飞行方向
      for (int i = 0; i < bulletCount; i++) {
        // 计算当前子弹的角度
        float angle = i * (360.0f / bulletCount) + rotationOffset;

        // 使用三角函数计算速度向量 (注意 Cocos2d-x 的 Y 轴向上为正)
        float vx = sin(CC_DEGREES_TO_RADIANS(angle)) * bulletSpeed;
        float vy = -cos(CC_DEGREES_TO_RADIANS(angle)) * bulletSpeed;

        // 创建子弹
        auto bullet = Sprite::create("Images/Bullet/bullet_enemy.png");
        if (!bullet)
          continue;

        bullet->setPosition(bossX, bossY); // 从 Boss 中心发射
        bullet->setScale(0.7f);
        bullet->setRotation(angle); // 让子弹旋转以匹配飞行方向

        // 使用 MoveBy 让子弹按向量移动
        auto move =
            MoveBy::create(travelTime, Vec2(vx * travelTime, vy * travelTime));

        auto finish = CallFuncN::create([=](Node *node) {
          node->removeFromParentAndCleanup(true);
          _enemyBullets.eraseObject(static_cast<Sprite *>(node));
        });

        bullet->runAction(Sequence::create(move, finish, nullptr));

        this->addChild(bullet, 0);
        _enemyBullets.pushBack(bullet);
      }

      // 递增旋转偏移，形成螺旋效果
      rotationOffset += 10.0f;
      if (rotationOffset >= 360.0f)
        rotationOffset -= 360.0f;

      continue; // Boss 发射完毕，跳过后续逻辑
    }

    // ==========================================
    // 情况 A：如果是精英怪 (Tag == 2) -> 发射散弹
    // ==========================================
    if (enemy->getTag() == 2) {

      // 定义三个方向 (左、中、右)
      float offsets[3] = {-100.0f, 0.0f, 100.0f};

      for (int i = 0; i < 3; i++) {
        // 使用你新找的 bullet_4.png (双发导弹)
        auto bullet = Sprite::create("Images/Bullet/bullet_4.png");
        if (!bullet)
          continue;

        bullet->setPosition(startX, startY);
        bullet->setScale(0.6f);
        bullet->setFlippedY(true); // 导弹头朝下

        // 计算终点
        float endX = startX + offsets[i];
        float endY = -50; // 飞到底部

        // 导弹速度
        auto move = MoveTo::create(2.0f, Vec2(endX, endY));

        // 必须小心移除
        auto finish = CallFuncN::create([=](Node *node) {
          node->removeFromParentAndCleanup(true);
          _enemyBullets.eraseObject(static_cast<Sprite *>(node));
        });

        bullet->runAction(Sequence::create(move, finish, nullptr));

        this->addChild(bullet, 0);
        _enemyBullets.pushBack(bullet);
      }

      // 播放导弹音效 (如果有的话)
      // SimpleAudioEngine::getInstance()->playEffect("Sound/missile.mp3");
    }

    // ==========================================
    // 情况 B：如果是普通怪 (Tag == 1) -> 发射普通红球
    // ==========================================
    else {
      // 定义子弹样式数组
      const std::string bulletImages[] = {
          "Images/Bullet/bullet_1.png", "Images/Bullet/bullet_2.png",
          "Images/Bullet/bullet_3.png", "Images/Bullet/bullet_4.png",
          "Images/Bullet/bullet_enemy.png"};

      // 随机索引 (0 到 4)
      int idx = cocos2d::RandomHelper::random_int(0, 4);
      auto bullet = Sprite::create(bulletImages[idx]);
      if (!bullet)
        continue;

      bullet->setPosition(startX, startY);
      bullet->setScale(0.8f);
      bullet->setFlippedY(true); // 让子弹头朝下

      auto move = MoveTo::create(1.5f, Vec2(startX, -50));

      auto finish = CallFuncN::create([=](Node *node) {
        node->removeFromParentAndCleanup(true);
        _enemyBullets.eraseObject(static_cast<Sprite *>(node));
      });

      bullet->runAction(Sequence::create(move, finish, nullptr));

      this->addChild(bullet, 0);
      _enemyBullets.pushBack(bullet);
    }
  }
}

void HelloWorld::removeBullet(Node *bullet) {
  if (bullet) {
    bullet->removeFromParentAndCleanup(true);
    _playerBullets.eraseObject(static_cast<Sprite *>(bullet));
  }
}

// ============================================================
// 7. 辅助函数 (GameOver, Explosion)
// ============================================================
void HelloWorld::spawnExplosion(Vec2 pos) {
  // 这里使用你之前改好的切图逻辑
  auto texture = Director::getInstance()->getTextureCache()->addImage(
      "Images/Effect/Effect.png");
  if (!texture)
    return;

  int cols = 3;
  int rows = 3;
  float frameWidth = texture->getContentSize().width / cols;
  float frameHeight = texture->getContentSize().height / rows;

  auto animation = Animation::create();
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      auto rect =
          Rect(c * frameWidth, r * frameHeight, frameWidth, frameHeight);
      auto frame = SpriteFrame::createWithTexture(texture, rect);
      animation->addSpriteFrame(frame);
    }
  }
  animation->setDelayPerUnit(0.05f);
  animation->setRestoreOriginalFrame(false);

  auto explosion = Sprite::createWithSpriteFrame(
      animation->getFrames().front()->getSpriteFrame());
  explosion->setPosition(pos);
  explosion->setScale(2.0f);
  this->addChild(explosion, 10);

  auto animate = Animate::create(animation);
  auto remove = RemoveSelf::create();
  SimpleAudioEngine::getInstance()->playEffect("Sound/explode.mp3");
  explosion->runAction(Sequence::create(animate, remove, nullptr));
}

void HelloWorld::gameOver() {
  // ==========================================
  // 1. 保存当前分数到 UserDefault
  // ==========================================
  UserDefault::getInstance()->setIntegerForKey("LastScore", _score);
  // 务必调用 flush 强制写入
  UserDefault::getInstance()->flush();

  // 停止背景音乐，但保留音效(让爆炸声播完)
  // SimpleAudioEngine::getInstance()->stopAllEffects();
  SimpleAudioEngine::getInstance()->stopBackgroundMusic();

  // ==========================================
  // 2. 切换到结算场景 (GameOverScene)
  // ==========================================
  auto scene = GameOverScene::createScene();
  Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
}

// BOSS
void HelloWorld::createBoss(float dt) {
  // 如果已有boss存活，不再生成新的
  if (_currentBoss != nullptr && _currentBoss->isAlive())
    return;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 创建Boss体积放大2倍，HP更高
  auto boss = BossEnemy::create("Images/Enemy/boss.png", 100);
  if (!boss)
    return;

  // 计算Boss碰撞体积
  float bossSize = boss->getContentSize().width * boss->getScaleX();

  // 直接随机生成boss位置
  float minX = origin.x + bossSize / 2;
  float maxX = origin.x + visibleSize.width - bossSize / 2;
  float randomX = minX + CCRANDOM_0_1() * (maxX - minX);
  Vec2 bossPos = Vec2(randomX, visibleSize.height + bossSize);

  // 只清理与boss位置重合的小敌机
  Rect bossRect(bossPos.x - bossSize / 2, bossPos.y - bossSize / 2, bossSize,
                bossSize);
  for (auto it = _enemies.begin(); it != _enemies.end();) {
    auto enemy = *it;
    // 只处理小敌机
    if (dynamic_cast<BossEnemy *>(enemy) == nullptr) {
      Rect enemyRect = enemy->getBoundingBox();
      if (bossRect.intersectsRect(enemyRect)) {
        // 与boss重合的小敌机掉血死亡+从场景移除+从数组删除
        enemy->takeDamage(enemy->getHP());
        this->removeChild(enemy);
        it = _enemies.erase(it);
      } else {
        it++;
      }
    } else {
      it++;
    }
  }

  // 放置boss并添加到场景
  boss->setPosition(bossPos);
  this->addChild(boss, 0);
  _enemies.pushBack(boss);

  // 标记当前boss，用于后续小敌机位置检查
  _currentBoss = boss;
  // 绑定boss死亡，死亡后清空指针
  boss->setOnDeathCallback([this]() {
    _currentBoss = nullptr; // 无boss时，小敌机无需位置检查
  });

  // boss移动
  boss->startMove(8.0f, origin.y + visibleSize.height * 0.7f);
}

// 检查boss位置是否与现有小敌机重合
bool HelloWorld::isPosOverlapWithBoss(Vec2 pos, float size) {
  // 无boss时，直接返回不重合
  if (_currentBoss == nullptr || !_currentBoss->isAlive()) {
    return false;
  }

  // 小敌机的碰撞矩形
  Rect enemyRect(pos.x - size / 2, pos.y - size / 2, size, size);
  // boss的碰撞矩形（考虑缩放）
  float bossSize = getBossSize();
  Vec2 bossPos = _currentBoss->getPosition();
  Rect bossRect(bossPos.x - bossSize / 2, bossPos.y - bossSize / 2, bossSize,
                bossSize);

  // 返回是否重合
  return enemyRect.intersectsRect(bossRect);
}

// 获取当前boss的碰撞尺寸
float HelloWorld::getBossSize() {
  if (_currentBoss == nullptr)
    return 0;
  return _currentBoss->getContentSize().width * _currentBoss->getScaleX();
}

void HelloWorld::addScore(int value) {
  _score += value;

  if (_scoreLabel) {
    // 1. 更新文字
    _scoreLabel->setString(StringUtils::format("Score: %d", _score));

    // ==========================================
    // 【新增】Q弹特效：让分数"跳"一下
    // ==========================================

    // 先停止之前的动作（防止连续得分时动作鬼畜）
    _scoreLabel->stopAllActions();

    // 恢复原始大小 (假设原始是 SystemFont 可能需要调整 scale)
    // 如果你 init 里没有设 scale，这里就是 1.0f
    _scoreLabel->setScale(1.0f);

    // 动作链：瞬间变大到 1.5倍 -> 0.2秒内弹回 1.0倍
    // EaseBackOut 会让回弹有一个"果冻"一样的物理效果，非常爽
    auto scaleUp = ScaleTo::create(0.0f, 1.5f);
    auto scaleDown = ScaleTo::create(0.2f, 1.0f);
    auto bounce = EaseBackOut::create(scaleDown);

    // 变颜色的闪烁效果 (闪一下金色)
    auto colorFlash = Sequence::create(
        TintTo::create(0.05f, Color3B::ORANGE),
        TintTo::create(0.2f, Color3B::YELLOW), // 假设平时是黄色
        nullptr);

    // 同时执行缩放和变色
    _scoreLabel->runAction(Spawn::create(
        Sequence::create(scaleUp, bounce, nullptr), colorFlash, nullptr));
  }
}

void HelloWorld::showFloatingScore(Vec2 pos, int score) {
  // 1. 创建临时的 Label
  std::string scoreStr = StringUtils::format("+%d", score);
  // 用加粗字体或者稍微大一点的字号
  auto label = Label::createWithTTF(scoreStr, "fonts/Marker Felt.ttf", 30);

  if (label) {
    // 2. 设在该死去的敌人的位置
    label->setPosition(pos);
    label->setColor(Color3B::YELLOW);
    // 加一点描边，让字在复杂的背景上也能看清
    label->enableOutline(Color4B::BLACK, 2);

    this->addChild(label, 200); // 层级要最高，盖住所有飞机

    // 3. 动作设计：向上飘 + 变大 + 透明度消失
    auto moveUp = MoveBy::create(0.8f, Vec2(0, 80)); // 向上飘 80 像素
    auto scaleBig = ScaleTo::create(0.1f, 1.5f);     // 瞬间变大
    auto scaleNormal = ScaleTo::create(0.3f, 1.0f);  // 变回原样
    auto fadeOut = FadeOut::create(0.3f);            // 最后 0.3秒 变透明

    // 组合动作
    // 前 0.5秒：只是飘 + 变大变小
    // 后 0.3秒：继续飘 + 变透明
    auto seq =
        Sequence::create(Spawn::create(moveUp,
                                       Sequence::create(scaleBig, scaleNormal,
                                                        DelayTime::create(0.2f),
                                                        fadeOut, nullptr),
                                       nullptr),
                         RemoveSelf::create(), // 飘完自己销毁，不占内存
                         nullptr);

    label->runAction(seq);
  }
}

void HelloWorld::checkItemCollisions() {
  // 如果主角死了，就别捡东西了
  if (_isPlayerDead || !_player)
    return;

  // 强转为 Player* 以便调用 upgradeFirepower
  Player *player = dynamic_cast<Player *>(_player);
  if (!player)
    return;

  // 遍历所有道具
  for (auto it = _items.begin(); it != _items.end();) {
    Item *item = *it;

    // 1. 优化：如果道具已经飞出屏幕（Y < -50），就清理掉
    if (item->getPositionY() < -50) {
      item->removeFromParent();
      it = _items.erase(it);
      continue;
    }

    // 2. 碰撞检测：主角撞到了道具
    if (player->getBoundingBox().intersectsRect(item->getBoundingBox())) {
      // === 根据类型触发效果 ===
      switch (item->getType()) {
      case ItemType::HP:
        // 加血 (之前在 BaseEntity 写的)
        player->heal(1);
        break;

      case ItemType::POWER:
        // 升级火力 (之前在 Player 写的)
        player->upgradeFirepower();
        break;

      case ItemType::SKIN:
        // 暂时当成“大分”来吃，加 500 分
        this->addScore(500);
        this->showFloatingScore(player->getPosition(), 500);
        break;
      }

      // === 播放音效 (可选) ===
      // SimpleAudioEngine::getInstance()->playEffect("Sound/get_item.mp3");

      // === 吃完后销毁道具 ===
      item->removeFromParent();
      it = _items.erase(it); // 从数组移除并指向下一个
    } else {
      // 没撞到，检查下一个
      ++it;
    }
  }
}