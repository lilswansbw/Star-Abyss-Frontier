
#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    cocos2d::Sprite* _bg1;
    cocos2d::Sprite* _bg2;

    cocos2d::Vector<cocos2d::Sprite*> _enemies; // 管理所有敌机的容器
    void createEnemy(float dt); // 创建单个敌机的函数
    void removeEnemy(cocos2d::Node* enemy); // 移除敌机的回调,避免内存泄漏
};

#endif // __HELLOWORLD_SCENE_H__
