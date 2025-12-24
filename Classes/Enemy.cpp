#include "Enemy.h"
#include "cocos2d.h"
#include "audio/include/SimpleAudioEngine.h"
USING_NS_CC;
using namespace CocosDenshion;
Enemy* Enemy::create(const std::string& imgPath, int hp)
{
    Enemy* enemy = new Enemy();
    if (enemy && enemy->init(imgPath, hp)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

bool Enemy::init(const std::string& imgPath, int hp)
{
    if (!Sprite::initWithFile(imgPath)) {
        return false;
    }
    _hp = hp;
    //_maxHP = hp;
    _isAlive = true;
    return true;
}

void Enemy::startMove(float duration, float endY)
{
    // 获取当前位置 (X轴已经在创建时设置好了)
    float currentX = this->getPositionX();

    // 1. 创建移动动作
    auto moveAction = cocos2d::MoveTo::create(duration, cocos2d::Vec2(currentX, endY));

    // 2. 创建清理动作
    auto removeWhenOut = cocos2d::CallFuncN::create([this](cocos2d::Node* node) {
        // 如果飞出屏幕还没死，就自我销毁
        if (this->_isAlive) {
            this->removeFromParentAndCleanup(true);
            // 标记死亡，以便 Scene 里的 Vector 下次清理时能识别
            this->_isAlive = false;
        }
        });

    // 3. 执行动作序列
    this->runAction(cocos2d::Sequence::create(moveAction, removeWhenOut, nullptr));
}

//被击中
void Enemy::hurt()
{
    if (!this->isAlive()) return;

    // 1. 扣血 (基类逻辑)
    this->takeDamage(1);

    // 2. [新增] 受击反馈动画：瞬间变红，然后变回原色
    if (this->isAlive()) {
        // TintTo: 变色 (时间, R, G, B)
        // 0.1秒变红 (255, 0, 0)
        auto tintRed = TintTo::create(0.1f, 255, 0, 0);
        // 0.1秒变回白色 (255, 255, 255) -> 在 Cocos 里白色意味着原图颜色
        auto tintBack = TintTo::create(0.1f, 255, 255, 255);

        this->runAction(Sequence::create(tintRed, tintBack, nullptr));
    }

    // 3. 死亡逻辑
    if (!this->isAlive()) {
        boom();
    }
}

//爆炸动画+销毁
void Enemy::boom()
{
    // 1. 停止移动
    this->stopAllActions();

    // 2. 加载爆炸资源
    auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Effect/Effect.png");
    if (!texture) {
        this->removeFromParentAndCleanup(true);
        return;
    }

    // 3. 准备动画帧
    int cols = 3;
    int rows = 3;
    float frameWidth = texture->getContentSize().width / cols;
    float frameHeight = texture->getContentSize().height / rows;

    auto animation = Animation::create();
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            auto rect = Rect(c * frameWidth, r * frameHeight, frameWidth, frameHeight);
            auto frame = SpriteFrame::createWithTexture(texture, rect);
            animation->addSpriteFrame(frame);
        }
    }
    animation->setDelayPerUnit(0.05f);
    animation->setRestoreOriginalFrame(false);

    // =================================================
    // 【核心修复】重置大小和方向！
    // =================================================

    // A. 解决“爆炸太大”：
    // 不管飞机之前缩放是 1.5 还是 2.0，爆炸时统一设为 1.2 倍 (你可以根据视觉效果微调)
    this->setScale(1.2f);

    // B. 解决“爆炸倒立”：
    // 之前有些飞机被我们 setFlippedY(true) 了，爆炸素材不需要倒过来，所以要复原
    this->setFlippedY(false);

    // C. 解决“变色”：
    // 如果之前用 setColor 变红了，爆炸应该是原色的，所以要重置颜色为白色
    this->setColor(Color3B::WHITE);

    // =================================================

    // 4. 播放动画并销毁
    auto animate = Animate::create(animation);
    auto removeSelf = CallFuncN::create([this](Node* node) {
        node->removeFromParentAndCleanup(true);
        this->_isAlive = false;
        });

    // 播放音效

    this->runAction(Sequence::create(animate, removeSelf, nullptr));
}