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
    // 1. 立即停止敌机的移动动作 (防止它一边炸一边飞)
    this->stopAllActions();

    // 2. 加载爆炸大图 (必须确保路径正确)
    auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Effect/Effect.png");

    // 安全检查：如果图片没找到，直接消失，防止崩溃
    if (!texture) {
        this->removeFromParentAndCleanup(true);
        return;
    }

    // 3. 按照 3x3 的网格切割图片 
    int cols = 3;
    int rows = 3;
    float frameWidth = texture->getContentSize().width / cols;
    float frameHeight = texture->getContentSize().height / rows;

    auto animation = Animation::create();

    // 遍历切图
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            auto rect = Rect(c * frameWidth, r * frameHeight, frameWidth, frameHeight);
            auto frame = SpriteFrame::createWithTexture(texture, rect);
            animation->addSpriteFrame(frame);
        }
    }

    animation->setDelayPerUnit(0.05f); // 播放速度
    animation->setRestoreOriginalFrame(false); 

    // 4. 创建动作序列
    auto animate = Animate::create(animation);

    auto removeSelf = CallFuncN::create([this](Node* node) {
        node->removeFromParentAndCleanup(true);
        // 如果你的类里有 _isAlive 标记，最好在这里也确认设为 false
        // this->_isAlive = false; 
        });

    // 播放音效 (可选，如果希望每架敌机爆炸都有声音)
    // SimpleAudioEngine::getInstance()->playEffect("Sound/explode.mp3");

    // 5. 执行动作：先变身爆炸，再销毁自己
    this->runAction(Sequence::create(animate, removeSelf, nullptr));
}