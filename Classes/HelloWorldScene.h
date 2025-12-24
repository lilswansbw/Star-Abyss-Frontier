#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Enemy.h"
#include "BossEnemy.h"
#include "Item.h"
class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual void update(float dt);
    virtual bool init();
    void addScore(int score);
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
    cocos2d::Label* _scoreLabel;
    int _score;

    bool _isPlayerDead; 

    cocos2d::Vector<Enemy*> _enemies; 
    cocos2d::Vector<cocos2d::Sprite*> _playerBullets; 
    cocos2d::Vector<cocos2d::Sprite*> _enemyBullets;

    cocos2d::Vector<Item*> _items;

    void checkItemCollisions();
    void createEnemy(float dt); 
    void removeEnemy(cocos2d::Node* enemy); 

    void checkCollisions(); 
    void spawnExplosion(cocos2d::Vec2 pos); 
    void gameOver();
    void playerShoot(float dt); 
    void enemyShoot(float dt);  
    void removeBullet(cocos2d::Node* bullet); 
    void showFloatingScore(cocos2d::Vec2 pos, int score);

    BossEnemy* _currentBoss;
    void createBoss(float dt); //生成Boss
    bool isPositionValid(Vec2 pos, float size); //检查位置是否与小敌机重合
    bool isPosOverlapWithBoss(Vec2 pos, float size);
    float getBossSize();
};

#endif // __HELLOWORLD_SCENE_H__
