#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "BaseEntity.h" // 继承自我们之前写的 BaseEntity

class Player : public BaseEntity {
public:
    CREATE_FUNC(Player);
    virtual bool init() override;
    cocos2d::Sprite* shoot(); 
    void startShoot();
    void stopShoot();
    virtual void onDeath() override;
    void upgradeFirepower();
    int getWeaponLevel() const { return _weaponLevel; }
    int getDamage() const;
private:
    void initTouchLogic(); // 内部函数：处理移动
    void autoShootLogic(float dt);
    int _weaponLevel;
};

#endif