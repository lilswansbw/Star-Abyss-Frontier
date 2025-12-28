#include "Player.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

// ==========================================
// 工厂方法：创建指定控制类型的玩家
// ==========================================
Player *Player::createWithControl(PlayerControlType controlType,
                                  int playerIndex) {
  Player *player = new (std::nothrow) Player();
  if (player && player->initWithControl(controlType, playerIndex)) {
    player->autorelease();
    return player;
  }
  CC_SAFE_DELETE(player);
  return nullptr;
}

// ==========================================
// 带控制类型的初始化
// ==========================================
bool Player::initWithControl(PlayerControlType controlType, int playerIndex) {
  _controlType = controlType;
  _playerIndex = playerIndex;

  // 1. 初始化父类
  if (!BaseEntity::init())
    return false;

  // ==========================================
  // [皮肤选择] 根据玩家编号选择不同皮肤
  // ==========================================
  // 注意：射击由 HelloWorldScene::playerShoot 统一管理，不在这里 startShoot

  std::string imgName;
  if (_playerIndex == 1) {
    // P1: 使用用户选择的皮肤
    int skinId = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
    imgName = StringUtils::format("Images/Player/player_%d.png", skinId);
  } else {
    // P2: 尝试使用 player_2.png，如果不存在则用默认并变色
    imgName = "Images/Player/player_2.png";
  }

  this->initWithFile(imgName);

  // 如果没图的保底逻辑
  if (this->getContentSize().width == 0) {
    this->initWithFile("Images/Player/player_1.png");
    // P2 加蓝色区分
    if (_playerIndex == 2) {
      this->setColor(Color3B(100, 150, 255));
    }
  }

  // 设置初始属性
  this->setHP(5);
  this->setupHPBar("Images/Effect/hp_bg.png", "Images/Effect/hp_.png", 0.15f);
  this->setTag(100 + _playerIndex); // P1=101, P2=102
  _weaponLevel = 1;

  // 统一缩放逻辑
  float targetGameWidth = 100.0f;
  float currentWidth = this->getContentSize().width;
  if (currentWidth > 0) {
    this->setScale(targetGameWidth / currentWidth);
  }

  // ==========================================
  // 根据控制类型初始化输入逻辑
  // ==========================================
  if (_controlType == PlayerControlType::MOUSE) {
    this->initTouchLogic();
  }
  // 键盘控制在 update 中处理移动

  // 初始化时空回溯状态
  _isRewinding = false;

  // 启用 update 调度
  this->scheduleUpdate();

  return true;
}

bool Player::init() {
  // 默认使用鼠标控制
  return initWithControl(PlayerControlType::MOUSE, 1);
}

void Player::startShoot() {
  this->schedule(CC_SCHEDULE_SELECTOR(Player::autoShootLogic), 0.2f,
                 CC_REPEAT_FOREVER, 0.0f);
}

void Player::stopShoot() {
  this->unschedule(CC_SCHEDULE_SELECTOR(Player::autoShootLogic));
}

void Player::initTouchLogic() {
  auto listener = EventListenerTouchOneByOne::create();
  listener->setSwallowTouches(true);

  listener->onTouchBegan = [](Touch *t, Event *e) { return true; };

  listener->onTouchMoved = [this](Touch *touch, Event *event) {
    if (!this->isAlive())
      return;

    auto winSize = Director::getInstance()->getWinSize();
    Vec2 newPos = this->getPosition() + touch->getDelta();

    // 边界计算
    float halfW = this->getBoundingBox().size.width / 2;
    float halfH = this->getBoundingBox().size.height / 2;

    // 限制 X
    if (newPos.x < halfW)
      newPos.x = halfW;
    if (newPos.x > winSize.width - halfW)
      newPos.x = winSize.width - halfW;

    // 限制 Y
    if (newPos.y < halfH)
      newPos.y = halfH;
    if (newPos.y > winSize.height - halfH)
      newPos.y = winSize.height - halfH;

    this->setPosition(newPos);
  };

  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Player::autoShootLogic(float dt) {
  if (!this->isAlive() || _hp <= 0) {
    return;
  }

  auto bullet = this->shoot();
  if (bullet) {
    if (!bullet->getParent()) {
      this->getParent()->addChild(bullet);
    }
  }
}

void Player::onDeath() {
  this->unscheduleAllCallbacks();
  this->stopAllActions();
  _hp = 0;
  _isAlive = false;
  this->setVisible(false);
}

// ==========================================
// [复活] 恢复玩家状态
// ==========================================
void Player::respawn(const Vec2 &position) {
  // 恢复生命值和状态
  this->setHP(3); // 复活后3点血
  _isAlive = true;
  _weaponLevel = 1; // 重置武器等级
  _isRewinding = false;
  _stateHistory.clear();

  // 恢复位置和可见性
  this->setPosition(position);
  this->setVisible(true);
  this->setOpacity(255);

  // 重新启用 update 调度
  this->scheduleUpdate();

  // 复活无敌闪烁效果（2秒）
  auto blink = Blink::create(2.0f, 8);
  this->runAction(blink);

  cocos2d::log("Player %d respawned!", _playerIndex);
}

cocos2d::Sprite *Player::shoot() {
  std::string soundFile = "Sound/click_001.mp3";
  std::string bulletImg = "Images/Bullet/bullet_player.png";
  float scale = 0.8f;
  float flyTime = 1.0f;

  switch (_weaponLevel) {
  case 2:
    soundFile = "Sound/click_002.mp3";
    bulletImg = "Images/Bullet/bullet_1.png";
    scale = 0.6f;
    flyTime = 0.8f;
    break;
  case 3:
    soundFile = "Sound/click_003.mp3";
    bulletImg = "Images/Bullet/bullet_2.png";
    scale = 0.5f;
    flyTime = 0.6f;
    break;
  default:
    break;
  }

  SimpleAudioEngine::getInstance()->playEffect(soundFile.c_str());

  auto bullet = Sprite::create(bulletImg);

  if (bullet) {
    float startX = this->getPositionX();
    float startY =
        this->getPositionY() + this->getBoundingBox().size.height / 2;
    bullet->setPosition(startX, startY);
    bullet->setScale(scale);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto move = MoveTo::create(flyTime, Vec2(startX, visibleSize.height + 200));
    auto remove = RemoveSelf::create();

    bullet->runAction(Sequence::create(move, remove, nullptr));
  }

  return bullet;
}

void Player::upgradeFirepower() {
  _weaponLevel++;

  if (_weaponLevel > 3) {
    _weaponLevel = 3;
    this->heal(5);
  }

  cocos2d::log("Player %d Weapon Upgraded! Current Level: %d", _playerIndex,
               _weaponLevel);
}

int Player::getDamage() const { return _weaponLevel; }

// ==========================================
// Update 函数 - 键盘移动 + 时空回溯
// ==========================================
void Player::update(float dt) {
  BaseEntity::update(dt);

  if (!this->isAlive())
    return;

  // ==========================================
  // [键盘控制] P2 使用 WASD 移动
  // ==========================================
  if (_controlType == PlayerControlType::KEYBOARD &&
      _moveDirection != Vec2::ZERO) {
    auto winSize = Director::getInstance()->getWinSize();
    Vec2 newPos = this->getPosition() + _moveDirection * _moveSpeed * dt;

    // 边界限制
    float halfW = this->getBoundingBox().size.width / 2;
    float halfH = this->getBoundingBox().size.height / 2;

    if (newPos.x < halfW)
      newPos.x = halfW;
    if (newPos.x > winSize.width - halfW)
      newPos.x = winSize.width - halfW;
    if (newPos.y < halfH)
      newPos.y = halfH;
    if (newPos.y > winSize.height - halfH)
      newPos.y = winSize.height - halfH;

    this->setPosition(newPos);
  }

  // ==========================================
  // [时空回溯] 状态记录与回溯
  // ==========================================
  if (!_isRewinding) {
    PlayerState state;
    state.position = this->getPosition();
    state.hp = _hp;
    state.timestamp = Director::getInstance()->getTotalFrames() / 60.0f;

    _stateHistory.push_back(state);

    if (_stateHistory.size() > MAX_HISTORY_FRAMES) {
      _stateHistory.pop_front();
    }
  } else {
    if (!_stateHistory.empty()) {
      createGhostTrail();

      PlayerState pastState = _stateHistory.back();
      _stateHistory.pop_back();

      this->setPosition(pastState.position);
      this->setHP(pastState.hp);

      this->setOpacity(180);
    } else {
      _isRewinding = false;
      this->setOpacity(255);
    }
  }
}

// ==========================================
// [残影效果] 创建玩家残影
// ==========================================
void Player::createGhostTrail() {
  auto ghost = Sprite::createWithTexture(this->getTexture());
  if (!ghost)
    return;

  ghost->setPosition(this->getPosition());
  ghost->setScale(this->getScale());
  ghost->setRotation(this->getRotation());
  ghost->setFlippedX(this->isFlippedX());
  ghost->setFlippedY(this->isFlippedY());

  ghost->setOpacity(150);
  ghost->setColor(Color3B(100, 180, 255));

  if (this->getParent()) {
    this->getParent()->addChild(ghost, this->getLocalZOrder() - 1);
  }

  auto fadeOut = FadeOut::create(0.3f);
  auto remove = RemoveSelf::create();
  ghost->runAction(Sequence::create(fadeOut, remove, nullptr));
}
