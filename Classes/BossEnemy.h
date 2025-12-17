#ifndef BossEnemy_H
#define BossEnemy_H
#include "Enemy.h"

class BossEnemy : public Enemy {
public:
    static BossEnemy* create(const std::string& imgPath, int hp);
    void startMove(float duration, float endY) override; //重写移动实现更慢
private:
    bool init(const std::string& imgPath, int hp) override;
};
#endif