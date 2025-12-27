#include "Player.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

bool Player::init() {
  // 1. 初始化父类
  if (!BaseEntity::init())
    return false;

  // ==========================================
  // [逻辑搬运] 读取皮肤 + 创建图片
  // ==========================================
  this->startShoot();
  int skinId = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
  std::string imgName =
      StringUtils::format("Images/Player/player_%d.png", skinId);

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
  _weaponLevel = 1;
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

  // 初始化时空回溯状态
  _isRewinding = false;

  return true;
}

void Player::startShoot() {
  // 【修改】去掉最后的 "shoot_key"，只保留前4个参数
  // 注意：Cocos2d-x 4.0 建议用 CC_REPEAT_FOREVER 代替 kRepeatForever
  this->schedule(CC_SCHEDULE_SELECTOR(Player::autoShootLogic), 0.2f,
                 CC_REPEAT_FOREVER, 0.0f);
}

void Player::stopShoot() {
  // 【修改】取消时，也要用选择器，不能用字符串
  this->unschedule(CC_SCHEDULE_SELECTOR(Player::autoShootLogic));
}

void Player::initTouchLogic() {
  auto listener = EventListenerTouchOneByOne::create();
  listener->setSwallowTouches(true); // 吞噬触摸，防止穿透

  listener->onTouchBegan = [](Touch *t, Event *e) { return true; };

  // [逻辑搬运] 移动与边界限制
  listener->onTouchMoved = [this](Touch *touch, Event *event) {
    if (!this->isAlive())
      return;

    auto winSize = Director::getInstance()->getWinSize();
    // 注意：这里直接用 this，因为代码就在飞机类里
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

cocos2d::Sprite *Player::shoot() {
  // ==========================================
  // 1. 准备素材配置 (根据等级选 音效 和 图片)
  // ==========================================
  std::string soundFile = "Sound/click_001.mp3";             // 默认音效
  std::string bulletImg = "Images/Bullet/bullet_player.png"; // 默认子弹
  float scale = 0.8f;                                        // 默认大小
  float flyTime = 1.0f;                                      // 默认飞行时间

  switch (_weaponLevel) {
  case 2:
    // 2级：金色子弹，声音变了，飞得快一点
    soundFile = "Sound/click_002.mp3";
    bulletImg = "Images/Bullet/bullet_1.png";
    scale = 0.6f; // 这种细长子弹不用太大
    flyTime = 0.8f;
    break;
  case 3:
    // 3级：大范围散射光，声音更猛，速度极快
    soundFile = "Sound/click_003.mp3";
    bulletImg = "Images/Bullet/bullet_2.png";
    scale = 0.5f; // 这个图本身很大，缩放一下
    flyTime = 0.6f;
    break;
  default:
    // 默认为1级配置
    break;
  }

  // ==========================================
  // 2. 播放对应的音效
  // ==========================================
  SimpleAudioEngine::getInstance()->playEffect(soundFile.c_str());

  // ==========================================
  // 3. 生成子弹
  // ==========================================
  auto bullet = Sprite::create(bulletImg);

  if (bullet) {
    // 设置位置 (从飞机头顶发射)
    float startX = this->getPositionX();
    float startY =
        this->getPositionY() + this->getBoundingBox().size.height / 2;
    bullet->setPosition(startX, startY);
    bullet->setScale(scale);

    // 创建飞行路径
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto move = MoveTo::create(flyTime, Vec2(startX, visibleSize.height + 200));
    auto remove = RemoveSelf::create();

    // 执行动作
    bullet->runAction(Sequence::create(move, remove, nullptr));
  }

  return bullet;
}

void Player::upgradeFirepower() {
  // 等级 +1
  _weaponLevel++;

  // 限制最高等级 (假设最高 3 级，防止无限变强)
  if (_weaponLevel > 3) {
    _weaponLevel = 3;
    // 如果已经是最高级，可以加分或者回满血作为替代奖励
    this->heal(5);
  }

  // 简单的控制台输出，帮你调试
  cocos2d::log("Player Weapon Upgraded! Current Level: %d", _weaponLevel);
}

int Player::getDamage() const {
  // 简单的数值策划
  // 1级=1点，2级=2点，3级=3点
  // 如果你想让3级特别猛，可以 return _weaponLevel * 2;
  return _weaponLevel;
}

// ==========================================
// [时空回溯] Update 函数 - 状态记录与回溯
// ==========================================
void Player::update(float dt) {
  // 调用父类update
  BaseEntity::update(dt);

  if (!this->isAlive())
    return;

  // 检测R键状态（通过EventKeyboard监听，这里简化处理）
  // 实际应用中需要在HelloWorldScene或其他地方监听键盘事件并设置_isRewinding

  if (!_isRewinding) {
    // ==========================================
    // 正常状态：记录当前帧状态
    // ==========================================
    PlayerState state;
    state.position = this->getPosition();
    state.hp = _hp;
    state.timestamp = Director::getInstance()->getTotalFrames() / 60.0f;

    _stateHistory.push_back(state);

    // 限制历史记录长度（保持最近3秒）
    if (_stateHistory.size() > MAX_HISTORY_FRAMES) {
      _stateHistory.pop_front();
    }
  } else {
    // ==========================================
    // 回溯状态：从历史记录中恢复
    // ==========================================
    if (!_stateHistory.empty()) {
      // 从队尾取出最近的历史状态
      PlayerState pastState = _stateHistory.back();
      _stateHistory.pop_back();

      // 应用历史状态
      this->setPosition(pastState.position);
      this->setHP(pastState.hp);

      // 视觉反馈：半透明表示回溯中
      this->setOpacity(180);
    } else {
      // 历史记录用完了，停止回溯
      _isRewinding = false;
      this->setOpacity(255); // 恢复不透明
    }
  }
}
