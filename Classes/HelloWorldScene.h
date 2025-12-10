
#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Enemy.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual void update(float dt);
    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
    cocos2d::Sprite* _bg1;
    cocos2d::Sprite* _bg2;

    cocos2d::Sprite* _stars1;
    cocos2d::Sprite* _stars2;

    cocos2d::Sprite* _player;
    bool _isPlayerDead; // 判断主角挂没挂，挂了就不能动了

    cocos2d::Vector<Enemy*> _enemies; // 管理所有敌机的容器

    cocos2d::Vector<cocos2d::Sprite*> _playerBullets; // 存我方子弹
    cocos2d::Vector<cocos2d::Sprite*> _enemyBullets;

    void createEnemy(float dt); // 创建单个敌机的函数
    void removeEnemy(cocos2d::Node* enemy); // 移除敌机的回调,避免内存泄漏

    void checkCollisions(); // 碰撞检测
    void spawnExplosion(cocos2d::Vec2 pos); // 在指定位置播放爆炸
    void gameOver();
    void playerShoot(float dt); // 我方射击
    void enemyShoot(float dt);  // 敌方射击 (控制所有敌人开火)
    void removeBullet(cocos2d::Node* bullet); // 子弹飞出屏幕后销毁

};

#endif // __HELLOWORLD_SCENE_H__
