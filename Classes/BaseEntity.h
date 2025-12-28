#ifndef __BASE_ENTITY_H__
#define __BASE_ENTITY_H__

#include "cocos2d.h"
#include <functional>

class BaseEntity : public cocos2d::Sprite {
public:
  virtual bool init() override;
  virtual void takeDamage(int damage);
  void heal(int amount);

  // 初始化血条
  void setupHPBar(const std::string &bgImg, const std::string &fillImg,
                  float scale = 0.2f);
  // 更新血条
  void updateHPBar();

  typedef std::function<void()> OnDeathCallback;
  void setOnDeathCallback(const OnDeathCallback &callback) {
    _onDeathCallback = callback;
  }
  bool isAlive() const { return _hp > 0; }
  void setHP(int hp) { _hp = hp; }
  int getHP() const { return _hp; }
  virtual ~BaseEntity() {}
  virtual void onDeath() {
    if (_onDeathCallback)
      _onDeathCallback();
  }

protected:
  int _hp;
  float _speed;
  bool _isAlive;
  float _originalScale;
  OnDeathCallback _onDeathCallback;

  cocos2d::Sprite *_hpBar;   // 血条前景
  cocos2d::Sprite *_hpBarBg; // 血条背景
  int _maxHp;                // 最大血量
};

#endif