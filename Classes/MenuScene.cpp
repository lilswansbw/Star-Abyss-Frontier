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

    // 1. 加背景（和游戏里一样）
    auto bg = Sprite::create("Images/Background/bg.jpg"); // 确保你有这张图
    if (bg) {
        // 简单的缩放适配
        float scaleX = visibleSize.width / bg->getContentSize().width;
        float scaleY = visibleSize.height / bg->getContentSize().height;
        bg->setScale(std::max(scaleX, scaleY));
        bg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        this->addChild(bg, -1);
    }

    // 2. 加游戏标题
    auto title = Label::createWithSystemFont("Star Abyss Frontier", "Arial", 64);
    title->setPosition(visibleSize.width / 2, visibleSize.height * 0.7);
    this->addChild(title, 1);

    // 3. 加开始按钮 (暂时用关闭图标代替，以后换成 Start.png)
    auto startItem = MenuItemImage::create(
        "CloseNormal.png",   // 正常状态图片
        "CloseSelected.png", // 按下状态图片
        CC_CALLBACK_1(MenuScene::menuStartCallback, this)); // 点击后执行哪个函数
    startItem->setScale(3.0f); // 放大点方便点
    startItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.4);

    // 4. 必须把按钮放进 Menu 容器里才能点击！
    auto menu = Menu::create(startItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

// 点击按钮后执行这个：
void MenuScene::menuStartCallback(Ref* pSender)
{
    // 创建游戏场景
    auto gameScene = HelloWorld::createScene();

    // 【核心知识点】使用 replaceScene 切换场景
    // TransitionFade 是加一个淡入淡出的特效，时间 1.0 秒
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, gameScene));
}