#include "Enemy.h"
#include "cocos2d.h"

USING_NS_CC;

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
    _isAlive = true;
    return true;
}

//被击中
void Enemy::hurt()
{
    if (!_isAlive) return; //已击毁则不处理

    _hp--;
    if (_hp <= 0) {
        _isAlive = false;
        boom(); //触发爆炸效果
    }
}

//爆炸动画+销毁
void Enemy::boom()
{
    //停止敌机当前所有动作
    this->stopAllActions();

    //加载爆炸帧动画
    Animation* explodeAnim = Animation::create();
    for (int i = 1; i <= 4; i++) { //先预设是4帧爆炸图
        std::string framePath = StringUtils::format("Images/Effect/explode_%d.png", i);
        explodeAnim->addSpriteFrameWithFile(framePath);
    }
    explodeAnim->setDelayPerUnit(0.1f); //每帧间隔0.1秒
    explodeAnim->setRestoreOriginalFrame(false); //播放完不回原帧

    //播放动画+播放后销毁敌机
    Animate* animate = Animate::create(explodeAnim);
    CallFuncN* removeSelf = CallFuncN::create([this](Node* node) {
        node->removeFromParentAndCleanup(true);
        });
    this->runAction(Sequence::create(animate, removeSelf, nullptr));
}