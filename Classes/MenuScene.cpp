#include "MenuScene.h"
#include "HelloWorldScene.h" // 引用游戏场景，不然没法跳过去

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

bool MenuScene::init()
{

    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bg = Sprite::create("Images/Menu/ezgif-frame-001.jpg");
    if (bg) {
        float scaleX = visibleSize.width / bg->getContentSize().width;
        float scaleY = visibleSize.height / bg->getContentSize().height;
        bg->setScale(std::max(scaleX, scaleY));
        bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        this->addChild(bg, -1); 

        auto animation = Animation::create();

        for (int i = 1; i <= 180; i++) {
            std::string name = StringUtils::format("Images/Menu/ezgif-frame-%03d.jpg", i);
            animation->addSpriteFrameWithFile(name);
        }

        animation->setDelayPerUnit(0.05f);
        animation->setRestoreOriginalFrame(true);

        auto animate = Animate::create(animation);
        bg->runAction(RepeatForever::create(animate));
    }
    else {
        CCLOG("Error: Menu background image not found!");
    }

    auto title = Label::createWithTTF("Star Abyss Frontier", "fonts/BlackOpsOne-Regular.ttf", 64);

    title->setTextColor(Color4B(0, 255, 255, 255)); 
    title->enableShadow(Color4B(0, 200, 255, 255), Size(0, 0), 20);
    title->enableOutline(Color4B::BLACK, 2);        
    title->enableGlow(Color4B(0, 200, 255, 255));   

    title->setPosition(visibleSize.width / 2, visibleSize.height * 0.7);
    this->addChild(title, 1);

    auto startItem = MenuItemImage::create(
        "CloseNormal.png",   // 正常状态图
        "CloseSelected.png", // 按下状态图
        CC_CALLBACK_1(MenuScene::menuStartCallback, this)); // 绑定回调

    if (startItem) {
        startItem->setScale(3.0f);
        startItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.4);
    }

    auto menu = Menu::create(startItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

void MenuScene::menuStartCallback(Ref* pSender)
{
    // 创建游戏场景
    auto gameScene = HelloWorld::createScene();

    // 切换场景 (带1秒淡入淡出特效)
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, gameScene));
}