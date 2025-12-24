#include "cocos2d.h"
#include "BaseEntity.h"

USING_NS_CC;

bool BaseEntity::init() {
    if (!Sprite::init()) return false;
    _hp = 1;
    _speed = 0;
    _isAlive = true;
    _maxHp = 1; // 默认最大血量
    _hpBar = nullptr; // 指针初始化，防崩溃
    _hpBarBg = nullptr;
    return true;
}

// BaseEntity.cpp

void BaseEntity::setupHPBar(const std::string& bgImg, const std::string& fillImg, float scale) {
    _maxHp = _hp;

    // 【解决 Bug 1】把传入的 scale 存起来，以后每次更新都用这个，不要用死数字
    _originalScale = scale;

    // 1. 创建背景
    _hpBarBg = Sprite::create(bgImg);
    if (_hpBarBg) {
        _hpBarBg->setPosition(this->getContentSize().width / 2, this->getContentSize().height + 15);
        _hpBarBg->setScaleX(scale);
        _hpBarBg->setScaleY(scale);
        _hpBarBg->setOpacity(180);
        this->addChild(_hpBarBg, 10);
    }

    // 2. 创建前景 (红条)
    _hpBar = Sprite::create(fillImg);
    if (_hpBar) {
        _hpBar->setAnchorPoint(Vec2(0, 0.5f));

        // 【解决 Bug 2】计算宽度修正比率
        // 逻辑：如果红条原图比背景原图宽，我们就要把红条额外缩小一点
        float widthRatio = 1.0f;
        if (_hpBarBg) {
            float bgWidth = _hpBarBg->getContentSize().width;
            float barWidth = _hpBar->getContentSize().width;
            // 目标宽度 / 实际宽度
            widthRatio = bgWidth / barWidth;
        }

        // 对齐位置
        if (_hpBarBg) {
            float bgRealWidth = _hpBarBg->getContentSize().width * _hpBarBg->getScaleX();
            float startX = _hpBarBg->getPositionX() - (bgRealWidth / 2);
            _hpBar->setPosition(startX, _hpBarBg->getPositionY());
        }

        // 【解决 Bug 2】应用修正比率
        // 最终缩放 = 用户设定的缩放(scale) * 图片宽度修正(widthRatio)
        _hpBar->setScaleX(scale * widthRatio);
        _hpBar->setScaleY(scale); // 高度通常不用修，或者你也乘以高度比率
        _hpBar->setOpacity(200);

        this->addChild(_hpBar, 11);
    }
}

void BaseEntity::updateHPBar() {
    if (_hpBar && _maxHp > 0) {
        // 计算百分比
        float percent = (float)_hp / (float)_maxHp;

        // 【解决 Bug 1】防止血量变成负数导致血条反向翻转
        if (percent < 0) percent = 0;
        if (percent > 1) percent = 1;

        // 【解决 Bug 2】更新时也要带上宽度比率
        // 为了简单，我们重新算一下宽度比率 (或者你在头文件再存一个 _widthRatio 变量会更高效)
        // 但这里为了不改头文件，我们再算一次
        float widthRatio = 1.0f;
        if (_hpBarBg) {
            widthRatio = _hpBarBg->getContentSize().width / _hpBar->getContentSize().width;
        }

        // 【解决 Bug 1】使用 _originalScale 而不是写死的 0.5
        _hpBar->setScaleX(_originalScale * widthRatio * percent);
    }
}

// BaseEntity.cpp

void BaseEntity::takeDamage(int damage) {
    if (_hp > 0) {
        _hp -= damage;

        // 1. 刷新血条 (之前的逻辑)
        updateHPBar();

        // ==========================================
        // 【新增】受伤视觉反馈：瞬间变红，然后恢复
        // ==========================================
        // 先停止之前的变色动作（防止连续挨打时颜色卡住）
        this->stopActionByTag(110);

        // 动作序列：0.1秒变红 -> 0.1秒变回白色
        auto flash = Sequence::create(
            TintTo::create(0.1f, Color3B::RED),
            TintTo::create(0.1f, Color3B::WHITE),
            nullptr
        );

        // 给动作设个 Tag，方便上面 stopActionByTag 找到它
        flash->setTag(110);
        this->runAction(flash);

        // ==========================================

        if (_hp <= 0) {
            _hp = 0;
            _isAlive = false;

            // 确保死的时候颜色是正常的白色，别死成红色的了
            this->setColor(Color3B::WHITE);

            onDeath();
        }
    }
}
void BaseEntity::heal(int amount) {
    // 只有活着才能回血
    if (!_isAlive) return;

    // 1. 增加血量
    _hp += amount;

    // 2. 限制上限 (防止溢出)
    if (_hp > _maxHp) {
        _hp = _maxHp;
    }

    // 3. 刷新血条
    updateHPBar();

    // 4. 【视觉反馈】全身闪一下绿色，表示治疗成功
    // TintTo 参数：时间, R, G, B
    auto tintGreen = TintTo::create(0.1f, 0, 255, 0);   // 变绿
    auto tintBack = TintTo::create(0.1f, 255, 255, 255); // 变回原色

    // 动作序列
    this->runAction(Sequence::create(tintGreen, tintBack, nullptr));
}