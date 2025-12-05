#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "ui/CocosGUI.h"
#include "cocos2d.h"

class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init(); // 初始化函数

    // 点击“开始按钮”时执行的函数
    void menuStartCallback(cocos2d::Ref* pSender);

    void menuExitCallback(cocos2d::Ref* pSender);

    CREATE_FUNC(MenuScene); // 必写的宏
private:
    cocos2d::Menu* _mainMenu;
};

#endif // __MENU_SCENE_H__