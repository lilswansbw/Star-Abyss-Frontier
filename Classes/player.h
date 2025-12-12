#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "BaseEntity.h" // 继承自我们之前写的 BaseEntity

class Player : public BaseEntity {
public:
    CREATE_FUNC(Player);
    
    virtual bool init() override;

    // 只需要对外暴露一个“射击”接口，场景不需要知道子弹长什么样
    cocos2d::Sprite* shoot(); 

    // 主角死亡逻辑
    virtual void onDeath() override;

private:
    void initTouchLogic(); // 内部函数：处理移动
};

#endif