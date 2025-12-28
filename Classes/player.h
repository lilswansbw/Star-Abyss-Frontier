#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "BaseEntity.h" // 继承自我们之前写的 BaseEntity
#include <deque>

// 时空回溯：记录玩家状态
struct PlayerState {
  cocos2d::Vec2 position;
  int hp;
  float timestamp;
};

// 玩家控制类型
enum class PlayerControlType {
  MOUSE,   // 鼠标控制 (P1)
  KEYBOARD // 键盘控制 WASD (P2)
};

class Player : public BaseEntity {
public:
  virtual void update(float dt) override;

  // 创建玩家（支持指定控制类型和玩家编号）
  static Player *createWithControl(PlayerControlType controlType,
                                   int playerIndex = 1);
  CREATE_FUNC(Player);

  virtual bool init() override;
  bool initWithControl(PlayerControlType controlType, int playerIndex);

  cocos2d::Sprite *shoot();
  void startShoot();
  void stopShoot();
  virtual void onDeath() override;
  void respawn(const cocos2d::Vec2 &position); // 复活玩家
  void upgradeFirepower();
  int getWeaponLevel() const { return _weaponLevel; }
  int getDamage() const;

  // 时空回溯控制接口
  void setRewinding(bool rewinding) { _isRewinding = rewinding; }
  bool isRewinding() const { return _isRewinding; }

  // 获取玩家信息
  int getPlayerIndex() const { return _playerIndex; }
  PlayerControlType getControlType() const { return _controlType; }

  // 键盘控制状态（供外部设置）
  void setMoveDirection(const cocos2d::Vec2 &dir) { _moveDirection = dir; }

private:
  // 时空回溯相关
  std::deque<PlayerState> _stateHistory;
  static const int MAX_HISTORY_FRAMES = 180;
  bool _isRewinding = false;

  // 残影效果
  void createGhostTrail();
  void initTouchLogic();    // 鼠标控制
  void initKeyboardLogic(); // 键盘控制 (WASD)
  void autoShootLogic(float dt);

  int _weaponLevel;
  int _playerIndex = 1; // 玩家编号 (1 或 2)
  PlayerControlType _controlType = PlayerControlType::MOUSE;
  cocos2d::Vec2 _moveDirection = cocos2d::Vec2::ZERO; // 键盘移动方向
  float _moveSpeed = 400.0f;                          // 键盘移动速度
};

#endif