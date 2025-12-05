#include "MenuScene.h"
#include "HelloWorldScene.h" 

USING_NS_CC;
using namespace cocos2d::ui;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

bool MenuScene::init()
{
    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // =================================================================
    // 1. 背景动画 (Ezgif 序列帧)
    // =================================================================
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

    // =================================================================
    // 2. 游戏标题
    // =================================================================
    auto title = Label::createWithTTF("Star Abyss Frontier", "fonts/Nabla-Regular-VariableFont_EDPT,EHLT.ttf", 64);
    if (!title) title = Label::createWithSystemFont("Star Abyss Frontier", "Arial", 64); // 防崩回退

    title->setTextColor(Color4B(0, 255, 255, 255));
    if (title->getTTFConfig().fontFilePath != "") { // 只有TTF才支持 Glow
        title->enableGlow(Color4B(0, 200, 255, 255));
    }
    title->enableOutline(Color4B::BLACK, 2);
    title->enableShadow(Color4B(0, 200, 255, 255), Size(0, 0), 20);
    title->setPosition(visibleSize.width / 2, visibleSize.height * 0.7);
    this->addChild(title, 1);

    // =================================================================
    // 3. 主菜单按钮 (开始/退出)
    // =================================================================
    auto startLabel = Label::createWithTTF("START GAME", "fonts/PermanentMarker-Regular.ttf", 48);
    if (!startLabel) startLabel = Label::createWithSystemFont("START GAME", "Arial", 48);
    auto startItem = MenuItemLabel::create(startLabel, CC_CALLBACK_1(MenuScene::menuStartCallback, this));
    startItem->setColor(Color3B::YELLOW);
    startItem->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(1.0f, 1.1f), ScaleTo::create(1.0f, 1.0f), nullptr)));

    auto exitLabel = Label::createWithTTF("EXIT GAME", "fonts/PermanentMarker-Regular.ttf", 48);
    if (!exitLabel) exitLabel = Label::createWithSystemFont("EXIT GAME", "Arial", 48);
    auto exitItem = MenuItemLabel::create(exitLabel, CC_CALLBACK_1(MenuScene::menuExitCallback, this));
    exitItem->setColor(Color3B::RED);

    _mainMenu = Menu::create(startItem, exitItem, NULL);
    _mainMenu->alignItemsVerticallyWithPadding(60);
    _mainMenu->setPosition(visibleSize.width / 2, visibleSize.height * 0.4);
    this->addChild(_mainMenu, 1);

    // =================================================================
    // 4. 装饰小飞机 (支持换肤交互)
    // =================================================================
    int savedSkin = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
    std::string savedName = StringUtils::format("Images/Player/player_%d.png", savedSkin);

    auto decorShip = Sprite::create(savedName);
    // 如果找不到 savedName，尝试默认图，防止第一次运行崩掉
    if (!decorShip) decorShip = Sprite::create("Images/Player/myplane.png");

    if (decorShip) {
        decorShip->setPosition(visibleSize.width * 0.15, visibleSize.height * 0.20);
        decorShip->setScale(1.5f);
        this->addChild(decorShip, 1);

        // 悬浮动画
        auto moveUp = MoveBy::create(1.5f, Vec2(0, 15));
        auto moveDown = moveUp->reverse();
        decorShip->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));

        // 点击事件：切换皮肤
        auto shipListener = EventListenerTouchOneByOne::create();
        shipListener->setSwallowTouches(true);
        shipListener->onTouchBegan = [decorShip](Touch* touch, Event* event) {
            Vec2 p = decorShip->convertToNodeSpace(touch->getLocation());
            Rect rect = Rect(0, 0, decorShip->getContentSize().width, decorShip->getContentSize().height);
            if (rect.containsPoint(p)) {
                // 切换逻辑：1->2->3->1
                int currentSkin = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
                currentSkin++;
                if (currentSkin > 3) currentSkin = 1; // 假设你有3个皮肤

                UserDefault::getInstance()->setIntegerForKey("SkinID", currentSkin);
                UserDefault::getInstance()->flush();

                // 换图
                std::string nextName = StringUtils::format("Images/Player/player_%d.png", currentSkin);
                // 这里加个安全判断，万一没有 player_2.png 就不换了，防止变白块
                if (FileUtils::getInstance()->isFileExist(nextName)) {
                    decorShip->setTexture(nextName);
                }

                // 特效
                decorShip->runAction(RotateBy::create(0.5f, 360));
                return true;
            }
            return false;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(shipListener, decorShip);
    }

    // =================================================================
    // 5. 设置按钮 (右上角 - 音效开关)
    // =================================================================
    bool isAudioOn = UserDefault::getInstance()->getBoolForKey("AudioOn", true);
    // std::string btnImg = isAudioOn ? "btn_music_on.png" : "btn_music_off.png"; 
    std::string btnImg = "CloseNormal.png"; // 临时替代，等你有了素材把上一行注释解开

    auto settingItem = MenuItemImage::create(
        btnImg, btnImg,
        [=](Ref* sender) {
            auto item = static_cast<MenuItemImage*>(sender);
            bool state = !UserDefault::getInstance()->getBoolForKey("AudioOn", true);
            UserDefault::getInstance()->setBoolForKey("AudioOn", state);
            UserDefault::getInstance()->flush();

            // 换图逻辑 (有了素材后再解开)
            // if (state) item->setNormalImage(Sprite::create("btn_music_on.png"));
            // else item->setNormalImage(Sprite::create("btn_music_off.png"));

            // 简单的视觉反馈
            item->runAction(Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 0.8f), nullptr));
        }
    );
    settingItem->setScale(0.8f);
    settingItem->setPosition(visibleSize.width - 50, visibleSize.height - 50);

    auto settingMenu = Menu::create(settingItem, NULL);
    settingMenu->setPosition(Vec2::ZERO);
    this->addChild(settingMenu, 10);

    return true;
}

void MenuScene::menuStartCallback(Ref* pSender)
{
    if (_mainMenu) _mainMenu->setVisible(false); // 隐藏主菜单

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 输入框
    auto textField = TextField::create("Your Name", "Arial", 48);
    textField->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.55));
    textField->setTextColor(Color4B::ORANGE);
    textField->setCursorEnabled(true);
    textField->setMaxLength(12);
    this->addChild(textField, 2);

    // 2. 确认按钮 (Confirm) - 这里补全了你之前漏掉的定义
    auto confirmLabel = Label::createWithTTF("LAUNCH !!", "fonts/PermanentMarker-Regular.ttf", 56);
    if (!confirmLabel) confirmLabel = Label::createWithSystemFont("LAUNCH !!", "Arial", 56);

    auto confirmItem = MenuItemLabel::create(confirmLabel, [=](Ref* sender) {
        // 保存名字
        std::string name = textField->getString();
        if (name.empty()) name = "Pilot";
        UserDefault::getInstance()->setStringForKey("PlayerName", name);
        UserDefault::getInstance()->flush();

        // 进游戏
        auto gameScene = HelloWorld::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, gameScene));
        });
    confirmItem->setColor(Color3B::YELLOW);
    confirmItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.35);

    // 3. 地图切换按钮 (Map Switch)
    int mapId = UserDefault::getInstance()->getIntegerForKey("MapID", 1);
    std::string mapText = (mapId == 1) ? "MAP: GALAXY" : "MAP: NEBULA";

    auto mapLabel = Label::createWithTTF(mapText, "fonts/BlackOpsOne-Regular.ttf", 40);
    if (!mapLabel) mapLabel = Label::createWithSystemFont(mapText, "Arial", 40);

    auto mapItem = MenuItemLabel::create(mapLabel, [mapLabel](Ref* sender) {
        int currId = UserDefault::getInstance()->getIntegerForKey("MapID", 1);
        currId = (currId == 1) ? 2 : 1;
        UserDefault::getInstance()->setIntegerForKey("MapID", currId);
        UserDefault::getInstance()->flush();
        mapLabel->setString((currId == 1) ? "MAP: GALAXY" : "MAP: NEBULA");
        });
    mapItem->setColor(Color3B::MAGENTA);
    mapItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.45);

    // 4. 统一放入菜单
    auto subMenu = Menu::create(confirmItem, mapItem, NULL);
    subMenu->setPosition(Vec2::ZERO);
    this->addChild(subMenu, 2);
}

void MenuScene::menuExitCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}