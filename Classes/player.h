#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "BaseEntity.h" // 继承自我们之前写的 BaseEntity
#include <deque>
// 时空回溯：记录玩家状态
struct PlayerState {
    cocos2d::Vec2 position;
    int hp;
    float timestamp;
};
class Player : public BaseEntity {
public:
    virtual void update(float dt) override;  // 需要 override BaseEntity 的 update
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
    // 时空回溯相关
    std::deque<PlayerState> _stateHistory;
    static const int MAX_HISTORY_FRAMES = 180;  // 3秒 (60fps)
    bool _isRewinding = false;
    void initTouchLogic(); // 内部函数：处理移动
    void autoShootLogic(float dt);
    int _weaponLevel;
};

#endif