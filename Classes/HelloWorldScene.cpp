#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// ��ʼ����������Ϸ��ʼʱִ��һ��
bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    //��ʼ����
    //�������Ƿɻ�
    auto player = Sprite::create("Images/player/myplane.png"); 
    if (player) {
        player->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 4 + origin.y));
        player->setTag(100); // ����������ǩ��100�������Ժ���
        this->addChild(player, 1);
    }

    // 2. �������������� (�÷ɻ�������ָ)
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    touchListener->onTouchMoved = [=](Touch* touch, Event* event) {
        // ��ȡ����
        auto target = this->getChildByTag(100);
        if (target) {
            // �÷ɻ�������ָ�ƶ�
            target->setPosition(target->getPosition() + touch->getDelta());
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    //������һ�ű���
    _bg1 = Sprite::create("Images/Background/bg.jpg");
    _bg1->setAnchorPoint(Vec2::ZERO);
    _bg1->setPosition(0, 0);

    // ���Ҫ��ͼƬ�Ŵ���ٱ�����������Ļ����
    float scaleX = visibleSize.width / _bg1->getContentSize().width;
    float scaleY = visibleSize.height / _bg1->getContentSize().height;
    // ȡ���͸��бȽϴ���Ǹ���������֤������Ļ�����ܻ�ü���һ��ߣ�
    float scale = std::max(scaleX, scaleY);

    _bg1->setScale(scale); // ִ�зŴ�
    this->addChild(_bg1, -1);

    //�����ڶ��ű���
    _bg2 = Sprite::create("Images/Background/bg.jpg");
    _bg2->setAnchorPoint(Vec2::ZERO);
    _bg2->setScale(scale); // �ڶ���ҲҪ�Ŵ�ͬ���ı���

    // ��Ϊͼ����ˣ��ڶ���ͼ������ڵ�һ��ͼ���Ŵ�󡱵�ͷ����
    // getBoundingBox().size.height ��ȡ���ǷŴ��ĸ߶�
    _bg2->setPosition(0, _bg1->getBoundingBox().size.height);

    this->addChild(_bg2, -1);

    this->schedule(schedule_selector(HelloWorld::createEnemy), 1.2f);

    return true;
}

//���������л����λ��+�����ƶ�
void HelloWorld::createEnemy(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //�����л�
    auto enemy = Sprite::create("Images/Enemy/eplane.png");
    if (!enemy) {
        enemy->setScale(2.0f);
    }

    //������õл���ʼX����,��Ҫ����Ļ���ȷ�Χ�ڣ����ⳬ���߽�
    float randomX = origin.x + CCRANDOM_0_1() * (visibleSize.width - enemy->getContentSize().width * enemy->getScaleX());
    //��ʼY���꣺����Ļ�Ϸ���࣬���Ա���ͻȻ����
    float startY = origin.y + visibleSize.height + enemy->getContentSize().height * enemy->getScaleY();
    enemy->setPosition(Vec2(randomX, startY));

    //������õл��ƶ��ٶ�2-4��Ӷ����ɵ���Ļ�ײ�
    float moveTime = 2.0f + CCRANDOM_0_1() * 2.0f;
    //�ƶ�Ŀ�꣬��Ļ�·���೬�����Ƴ�
    float endY = origin.y - enemy->getContentSize().height * enemy->getScaleY();
    auto moveAction = MoveTo::create(moveTime, Vec2(randomX, endY));

    //�л��ɳ���Ļ���Զ��Ƴ������ڴ�й©
    auto removeAction = CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeEnemy, this));
    auto sequence = Sequence::create(moveAction, removeAction, nullptr);
    enemy->runAction(sequence);

    //���ӵл��������͹�������
    this->addChild(enemy, 0); // �㼶0���ڱ���֮�ϡ�����֮��
    _enemies.pushBack(enemy);
}

//�Ƴ��л����ӳ�����������ͬʱɾ��
void HelloWorld::removeEnemy(cocos2d::Node* enemy)
{
    if (enemy) {
        // �ӳ������Ƴ�
        enemy->removeFromParentAndCleanup(true);
        // ���������Ƴ�
        _enemies.eraseObject(static_cast<Sprite*>(enemy));
    }
}

