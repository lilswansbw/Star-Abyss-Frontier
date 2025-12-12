#ifndef Enemy_H
#define Enemy_H

#include "cocos2d.h"

USING_NS_CC;

class Enemy : public Sprite
{
public:
    //创建敌机参数,敌机图片和初始血量）
    static Enemy* create(const std::string& imgPath, int hp);

    void startMove(float duration, float endY);

    // 敌机被击中，减血+判断是否击毁
    void hurt();

    //获取它的当前状态
    int getHp() const { return _hp; }
    bool isAlive() const { return _isAlive; }

private:
    bool init(const std::string& imgPath, int hp); //初始化
    void boom(); //播放爆炸动画+销毁敌机

    int _hp;//敌机血量
    bool _isAlive; // 是否存活
};

#endif // Enemy_H