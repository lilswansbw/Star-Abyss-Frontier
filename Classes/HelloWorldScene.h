#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "BossEnemy.h"
#include "Enemy.h"
#include "Item.h"
#include "Player.h"
#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene {
public:
  // 支持玩家模式参数 (1=单人, 2=双人)
  static cocos2d::Scene *createScene(int playerMode = 1);
  static HelloWorld *createWithMode(int playerMode);

  virtual void update(float dt);
  virtual bool init();
  bool initWithMode(int playerMode);

  void addScore(int score);
  void menuCloseCallback(cocos2d::Ref *pSender);

  CREATE_FUNC(HelloWorld);

private:
  // 玩家模式 (1=单人, 2=双人)
  int _playerMode = 1;

  // 多玩家支持
  cocos2d::Vector<Player *> _players;

  // P2 键盘状态
  bool _keyW = false;
  bool _keyA = false;
  bool _keyS = false;
  bool _keyD = false;

  // 背景
  cocos2d::Sprite *_bg1;
  cocos2d::Sprite *_bg2;
  cocos2d::Sprite *_stars1;
  cocos2d::Sprite *_stars2;

  // 旧的单玩家指针（保持兼容）
  cocos2d::Sprite *_player;

  cocos2d::Label *_scoreLabel;
  int _score;

  bool _isPlayerDead;
  bool _isShiftPressed;

  cocos2d::Vector<Enemy *> _enemies;
  cocos2d::Vector<cocos2d::Sprite *> _playerBullets;
  cocos2d::Vector<cocos2d::Sprite *> _enemyBullets;
  cocos2d::Vector<Item *> _items;

  void checkItemCollisions();
  void createEnemy(float dt);
  void removeEnemy(cocos2d::Node *enemy);

  void checkCollisions();
  void spawnExplosion(cocos2d::Vec2 pos, bool playSound = true);
  void gameOver();
  void playerShoot(float dt);
  void enemyShoot(float dt);
  void removeBullet(cocos2d::Node *bullet);
  void showFloatingScore(cocos2d::Vec2 pos, int score);

  BossEnemy *_currentBoss;
  void createBoss(float dt);
  bool isPositionValid(Vec2 pos, float size);
  bool isPosOverlapWithBoss(Vec2 pos, float size);
  float getBossSize();

  // 更新 P2 移动方向
  void updateP2MoveDirection();

  // 检查是否所有玩家都死亡
  bool areAllPlayersDead();

  // 调度玩家复活
  void schedulePlayerRespawn(Player *player);
};

#endif // __HELLOWORLD_SCENE_H__
