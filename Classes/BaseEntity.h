#ifndef __BASE_ENTITY_H__
#define __BASE_ENTITY_H__

#include "cocos2d.h"

class BaseEntity : public cocos2d::Sprite {
public:
    virtual bool init() override; 

    // 核心通用属性
    CC_SYNTHESIZE(int, _hp, HP); // 自动生成 getHP/setHP
    CC_SYNTHESIZE(float, _speed, Speed);

    // 通用接口：受伤
    virtual void takeDamage(int damage);

    // 通用接口：是否存活
    bool isAlive() const { return _hp > 0; }

    // 通用接口：死亡处理 
    virtual void onDeath() = 0;
};

#endif