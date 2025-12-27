#ifndef __GAMEOVER_SCENE_H__
#define __GAMEOVER_SCENE_H__

#include "cocos2d.h"

class GameOverScene : public cocos2d::Scene {
public:
  static cocos2d::Scene *createScene();

  virtual bool init();

  // 实现 create() 宏
  CREATE_FUNC(GameOverScene);
};

#endif // __GAMEOVER_SCENE_H__
