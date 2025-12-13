#ifndef __BASE_ENTITY_H__
#define __BASE_ENTITY_H__

#include "cocos2d.h"

class BaseEntity : public cocos2d::Sprite {
public:
    virtual bool init() override;

    // 这就是 takeDamage 的定义处
    virtual void takeDamage(int damage);

    bool isAlive() const { return _hp > 0; }
    void setHP(int hp) { _hp = hp; }
    int getHP() const { return _hp; }
    virtual ~BaseEntity() {}
    virtual void onDeath() {}

protected:
    int _hp;
    float _speed;
    bool _isAlive;
};

#endif