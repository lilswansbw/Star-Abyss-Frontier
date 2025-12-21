#ifndef __BASE_ENTITY_H__
#define __BASE_ENTITY_H__

#include "cocos2d.h"
#include <functional>

class BaseEntity : public cocos2d::Sprite {
public:
    virtual bool init() override;
    virtual void takeDamage(int damage);

    // 【新增】初始化血条 (子类在 init 时调用这个就能显示血条)
    void setupHPBar(const std::string& bgImg, const std::string& fillImg, float scale = 0.2f);

    // 【新增】更新血条 (内部调用，无需对外暴露，但为了简单先放public)
    void updateHPBar();

    // ... (你原有的回调和GetSet保持不变) ...
    typedef std::function<void()> OnDeathCallback;
    void setOnDeathCallback(const OnDeathCallback& callback) { _onDeathCallback = callback; }
    bool isAlive() const { return _hp > 0; }
    void setHP(int hp) { _hp = hp; }
    int getHP() const { return _hp; }
    virtual ~BaseEntity() {}
    virtual void onDeath() { if (_onDeathCallback) _onDeathCallback(); }

protected:
    int _hp;
    float _speed;
    bool _isAlive;
    float _originalScale; 
    OnDeathCallback _onDeathCallback;

    // 【关键】取消注释，并把 _maxHP 用起来
    cocos2d::Sprite* _hpBar;    // 前景 (绿条/红条)
    cocos2d::Sprite* _hpBarBg;  // 背景 (黑底)
    int _maxHp;                 // 记录最大血量，用于计算百分比
};

#endif