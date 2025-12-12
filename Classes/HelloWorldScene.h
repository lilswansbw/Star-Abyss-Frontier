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
    bool _isPlayerDead; // �ж����ǹ�û�ң����˾Ͳ��ܶ���

    cocos2d::Vector<Enemy*> _enemies; // �������ел�������

    cocos2d::Vector<cocos2d::Sprite*> _playerBullets; // ���ҷ��ӵ�
    cocos2d::Vector<cocos2d::Sprite*> _enemyBullets;

    void createEnemy(float dt); // ���������л��ĺ���
    void removeEnemy(cocos2d::Node* enemy); // �Ƴ��л��Ļص�,�����ڴ�й©

    void checkCollisions(); // ��ײ���
    void spawnExplosion(cocos2d::Vec2 pos); // ��ָ��λ�ò��ű�ը
    void gameOver();
    void playerShoot(float dt); // �ҷ����
    void enemyShoot(float dt);  // �з���� (�������е��˿���)
    void removeBullet(cocos2d::Node* bullet); // �ӵ��ɳ���Ļ������

};

#endif // __HELLOWORLD_SCENE_H__
