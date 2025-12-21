#include "BossEnemy.h"

BossEnemy* BossEnemy::create(const std::string& imgPath, int hp) {
    BossEnemy* enemy = new BossEnemy();
    if (enemy && enemy->init(imgPath, hp)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

bool BossEnemy::init(const std::string& imgPath, int hp) {
    // 调用父类初始化
    if (!Enemy::init(imgPath, hp)) return false;

    this->setScale(2.0f); // 变大

    this->setupHPBar("Images/Effect/hp_bg.png", "Images/Effect/hp_.png", 0.4f);

    return true;
}

void BossEnemy::startMove(float duration, float endY) {
    // 1. 获取当前位置
    float currentX = this->getPositionX();

    // 2. 入场动作：从屏幕外飞到初始位置 (endY)
    auto moveEnter = MoveTo::create(duration, Vec2(currentX, endY));

    // 3. 【关键】入场结束后，执行 moveRandomly 函数开启无限巡逻
    auto startPatrol = CallFunc::create([this]() {
        this->moveRandomly();
        });

    // 4. 执行序列
    auto seq = Sequence::create(moveEnter, startPatrol, nullptr);
    this->runAction(seq);
}

void BossEnemy::moveRandomly() {
    // 如果 Boss 死了，就停止移动（虽然对象可能还没销毁，但逻辑上要停）
    // 注意：需要在 BaseEntity 加一个 isAlive() 判断，或者直接判断指针安全
    // 这里为了简单直接跑逻辑

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ==========================================
    // [算法核心] 计算随机移动的目标点 (边界限制)
    // ==========================================

    // 1. 定义 Boss 的活动范围 (屏幕上半部分)
    // 比如：X轴在屏幕内，Y轴在屏幕高度的 50% ~ 90% 之间
    float margin = 50.0f; // 留点边距，别贴着墙
    float minX = margin;
    float maxX = visibleSize.width - margin;
    float minY = visibleSize.height * 0.5f; // 最低飞到屏幕中间
    float maxY = visibleSize.height * 0.9f; // 最高飞到屏幕顶部附近

    // 2. 生成随机坐标
    float randomX = minX + CCRANDOM_0_1() * (maxX - minX);
    float randomY = minY + CCRANDOM_0_1() * (maxY - minY);
    Vec2 targetPos = Vec2(randomX, randomY);

    // 3. 计算移动速度
    // 为了让移动看起来自然，速度应该是恒定的，而不是时间恒定
    // 距离 = (目标点 - 当前点) 的长度
    float distance = this->getPosition().distance(targetPos);
    float speed = 100.0f; // 每秒移动 100 像素 (数值越小越慢，看起来越沉重)
    float duration = distance / speed;

    // ==========================================
    // [动作链] 移动 -> 停顿一小会 -> 再找下一个点 (递归)
    // ==========================================

    auto move = MoveTo::create(duration, targetPos);

    // 稍微带一点缓动效果，让 Boss 看起来有惯性 (EaseSineInOut)
    auto easeMove = EaseSineInOut::create(move);

    // 到了新位置后，发呆 0.5 ~ 1.5 秒，再进行下一次移动
    float waitTime = 0.5f + CCRANDOM_0_1() * 1.0f;
    auto delay = DelayTime::create(waitTime);

    // 【递归核心】动作做完后，再次调用 moveRandomly
    auto nextMove = CallFunc::create([this]() {
        this->moveRandomly();
        });

    // 执行序列
    auto seq = Sequence::create(easeMove, delay, nextMove, nullptr);
    this->runAction(seq);
}