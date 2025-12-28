#ifndef BossEnemy_H
#define BossEnemy_H
#include "Enemy.h"

class BossEnemy : public Enemy {
public:
  static BossEnemy *create(const std::string &imgPath, int hp);
  void startMove(float duration, float endY) override;

protected:
  bool init(const std::string &imgPath, int hp);
  void moveRandomly();
  float _speed;
  bool _isAlive;
};
#endif