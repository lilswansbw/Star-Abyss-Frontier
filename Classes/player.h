#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "BaseEntity.h"
#include <deque>

// 状态记录（用于时光回溯）
struct PlayerState {
  cocos2d::Vec2 position;
  int hp;
  float timestamp;
};

// 控制类型
enum class PlayerControlType {
  MOUSE,   // 鼠标 (P1)
  KEYBOARD // 键盘 WASD (P2)
};

class Player : public BaseEntity {
public:
  virtual void update(float dt) override;
  static Player *createWithControl(PlayerControlType controlType,
                                   int playerIndex = 1);
  CREATE_FUNC(Player);

  virtual bool init() override;
  bool initWithControl(PlayerControlType controlType, int playerIndex);

  cocos2d::Sprite *shoot();
  void startShoot();
  void stopShoot();
  virtual void onDeath() override;
  void respawn(const cocos2d::Vec2 &position);
  void upgradeFirepower();
  int getWeaponLevel() const { return _weaponLevel; }
  int getDamage() const;

  void setRewinding(bool rewinding) { _isRewinding = rewinding; }
  bool isRewinding() const { return _isRewinding; }
  int getPlayerIndex() const { return _playerIndex; }
  PlayerControlType getControlType() const { return _controlType; }
  void setMoveDirection(const cocos2d::Vec2 &dir) { _moveDirection = dir; }

private:
  std::deque<PlayerState> _stateHistory;
  static const int MAX_HISTORY_FRAMES = 180;
  bool _isRewinding = false;

  void createGhostTrail();
  void initTouchLogic();
  void initKeyboardLogic();
  void autoShootLogic(float dt);

  int _weaponLevel;
  int _playerIndex = 1;
  PlayerControlType _controlType = PlayerControlType::MOUSE;
  cocos2d::Vec2 _moveDirection = cocos2d::Vec2::ZERO;
  float _moveSpeed = 400.0f;
};

#endif