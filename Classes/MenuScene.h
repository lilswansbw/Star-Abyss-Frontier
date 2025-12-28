#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"


class MenuScene : public cocos2d::Scene {
public:
  static cocos2d::Scene *createScene();
  virtual bool init();

  void menuStartCallback(cocos2d::Ref *pSender);
  void menuExitCallback(cocos2d::Ref *pSender);

  CREATE_FUNC(MenuScene);

private:
  cocos2d::Menu *_mainMenu;
  int _playerMode = 1;        // 玩家模式 (1=单人, 2=双人)
  cocos2d::Label *_modeLabel; // 模式显示标签
};

#endif // __MENU_SCENE_H__