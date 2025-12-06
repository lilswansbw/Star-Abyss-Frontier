#include "MenuScene.h"
#include "HelloWorldScene.h" 
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace CocosDenshion; // 引用音频命名空间

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

bool MenuScene::init()
{
    if (!Scene::init()) return false;

    SimpleAudioEngine::getInstance()->preloadEffect("Sound/click_001.ogg");
    SimpleAudioEngine::getInstance()->preloadEffect("Sound/launch.ogg");

    // 播放菜单循环背景音乐
    if (!SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying()) {
        SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/menu_bgm.mp3", true);
    }
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.1f);
    SimpleAudioEngine::getInstance()->setEffectsVolume(1.0f);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // --------------------------------------------------------
    // 1. 背景动画
    // --------------------------------------------------------
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

    // --------------------------------------------------------
    // 2. 标题
    // --------------------------------------------------------
    auto title = Label::createWithTTF("Star Abyss Frontier", "fonts/Nabla-Regular-VariableFont_EDPT,EHLT.ttf", 64);
    if (!title) title = Label::createWithSystemFont("Star Abyss Frontier", "Arial", 64);

    title->setTextColor(Color4B(0, 255, 255, 255));
    title->enableShadow(Color4B(0, 200, 255, 255), Size(0, 0), 20);
    title->enableOutline(Color4B::BLACK, 2);
 
    if (title->getTTFConfig().fontFilePath != "") {
        title->enableGlow(Color4B(0, 200, 255, 255));
    }
    title->setPosition(visibleSize.width / 2, visibleSize.height * 0.7);
    this->addChild(title, 1);

    // --------------------------------------------------------
    // 3. 主菜单按钮
    // --------------------------------------------------------
    auto startLabel = Label::createWithTTF("START GAME", "fonts/PermanentMarker-Regular.ttf", 48);
    if (!startLabel) startLabel = Label::createWithSystemFont("START GAME", "Arial", 48);

    auto startItem = MenuItemLabel::create(startLabel, [=](Ref* sender) {
        SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.mp3");
        this->menuStartCallback(sender);
        });

    startItem->setColor(Color3B::YELLOW);
    startItem->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(1.0f, 1.1f), ScaleTo::create(1.0f, 1.0f), nullptr)));

    auto exitLabel = Label::createWithTTF("EXIT GAME", "fonts/PermanentMarker-Regular.ttf", 48);
    if (!exitLabel) exitLabel = Label::createWithSystemFont("EXIT GAME", "Arial", 48);

    // 点击退出：播放音效 -> 执行回调
    auto exitItem = MenuItemLabel::create(exitLabel, [=](Ref* sender) {
        SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.mp3");
        this->menuExitCallback(sender);
        });
    exitItem->setColor(Color3B::RED);

    _mainMenu = Menu::create(startItem, exitItem, NULL);
    _mainMenu->alignItemsVerticallyWithPadding(60);
    _mainMenu->setPosition(visibleSize.width / 2, visibleSize.height * 0.4);
    this->addChild(_mainMenu, 1);

    // --------------------------------------------------------
    // 4. 装饰飞机 (换肤)
    // --------------------------------------------------------
    int savedSkin = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
    std::string savedName = StringUtils::format("Images/Player/player_%d.png", savedSkin);
    auto decorShip = Sprite::create(savedName);
    if (!decorShip) decorShip = Sprite::create("Images/Player/myplane.png");

    if (decorShip) {
        decorShip->setPosition(visibleSize.width * 0.15, visibleSize.height * 0.20);

        // 统一缩放逻辑
        float targetWidth = 120.0f;
        float currentWidth = decorShip->getContentSize().width;
        decorShip->setScale(targetWidth / currentWidth);

        this->addChild(decorShip, 1);

        auto moveUp = MoveBy::create(1.5f, Vec2(0, 15));
        auto moveDown = moveUp->reverse();
        decorShip->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));

        auto shipListener = EventListenerTouchOneByOne::create();
        shipListener->setSwallowTouches(true);
        shipListener->onTouchBegan = [decorShip](Touch* touch, Event* event) {
            Vec2 p = decorShip->convertToNodeSpace(touch->getLocation());
            Rect rect = Rect(0, 0, decorShip->getContentSize().width, decorShip->getContentSize().height);

            if (rect.containsPoint(p)) {
                // 点击音效
                SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.ogg");

                int currentSkin = UserDefault::getInstance()->getIntegerForKey("SkinID", 1);
                currentSkin++;
                if (currentSkin > 3) currentSkin = 1;

                UserDefault::getInstance()->setIntegerForKey("SkinID", currentSkin);
                UserDefault::getInstance()->flush();

                std::string nextName = StringUtils::format("Images/Player/player_%d.png", currentSkin);
                if (FileUtils::getInstance()->isFileExist(nextName)) {
                    decorShip->setTexture(nextName);
                    // 换图后重置缩放
                    float targetW = 120.0f;
                    float newW = decorShip->getContentSize().width;
                    decorShip->setScale(targetW / newW);
                }
                decorShip->runAction(RotateBy::create(0.5f, 360));
                return true;
            }
            return false;
            };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(shipListener, decorShip);
    }

    // --------------------------------------------------------
    // 5. 设置按钮 (控制音乐开关)
    // --------------------------------------------------------
    bool isAudioOn = UserDefault::getInstance()->getBoolForKey("AudioOn", true);
    std::string btnImg = isAudioOn ? "Images/Menu/audioOn.png" : "Images/Menu/audioOff.png";
    auto settingItem = MenuItemImage::create(
        btnImg, btnImg,
        [=](Ref* sender) {
            auto item = static_cast<MenuItemImage*>(sender);
            bool state = !UserDefault::getInstance()->getBoolForKey("AudioOn", true);
            UserDefault::getInstance()->setBoolForKey("AudioOn", state);
            UserDefault::getInstance()->flush();

            // 点击音效
            SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.mp3");

            // 控制音乐暂停/恢复
            if (state) {
                item->setNormalImage(Sprite::create("Images/Menu/audioOn.png")); // 换图
                SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
            }
            else {
                item->setNormalImage(Sprite::create("Images/Menu/audioOff.png")); // 换图
                SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
            }

            item->runAction(Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 0.8f), nullptr));
        }
    );
    settingItem->setScale(2.0f);
    settingItem->setPosition(visibleSize.width - 50, visibleSize.height - 50);

    auto settingMenu = Menu::create(settingItem, NULL);
    settingMenu->setPosition(Vec2::ZERO);
    this->addChild(settingMenu, 10);

    return true;
}

// --------------------------------------------------------
// 点击开始后
// --------------------------------------------------------
void MenuScene::menuStartCallback(Ref* pSender)
{
    if (_mainMenu) _mainMenu->setVisible(false);

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 输入框
    auto textField = TextField::create("Your Name", "Arial", 48);
    textField->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.55));
    textField->setTextColor(Color4B::ORANGE);
    textField->setCursorEnabled(true);
    textField->setMaxLength(12);
    this->addChild(textField, 2);

    // 2. 确认出击按钮
    auto confirmLabel = Label::createWithTTF("LAUNCH !!", "fonts/PermanentMarker-Regular.ttf", 56);
    if (!confirmLabel) confirmLabel = Label::createWithSystemFont("LAUNCH !!", "Arial", 56);

    auto confirmItem = MenuItemLabel::create(confirmLabel, [=](Ref* sender) {
        // --- [修改] 点击LAUNCH后的逻辑 ---

        // 1. 播放出击音效
        SimpleAudioEngine::getInstance()->playEffect("Sound/launch.ogg");

        // 2. 停止菜单音乐 (防止和游戏音乐重叠)
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();

        // 3. 保存名字
        std::string name = textField->getString();
        if (name.empty()) name = "Pilot";
        UserDefault::getInstance()->setStringForKey("PlayerName", name);
        UserDefault::getInstance()->flush();

        // 4. 进游戏
        auto gameScene = HelloWorld::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, gameScene));
        });
    confirmItem->setColor(Color3B::YELLOW);
    confirmItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.35);

    // 3. 地图切换按钮
    int mapId = UserDefault::getInstance()->getIntegerForKey("MapID", 1);
    std::string mapText = (mapId == 1) ? "MAP: GALAXY" : "MAP: NEBULA";

    auto mapLabel = Label::createWithTTF(mapText, "fonts/BlackOpsOne-Regular.ttf", 40);
    if (!mapLabel) mapLabel = Label::createWithSystemFont(mapText, "Arial", 40);

    auto mapItem = MenuItemLabel::create(mapLabel, [mapLabel](Ref* sender) {
        // 点击音效
        SimpleAudioEngine::getInstance()->playEffect("Sound/click_001.mp3");

        int currId = UserDefault::getInstance()->getIntegerForKey("MapID", 1);
        currId = (currId == 1) ? 2 : 1;
        UserDefault::getInstance()->setIntegerForKey("MapID", currId);
        UserDefault::getInstance()->flush();
        mapLabel->setString((currId == 1) ? "MAP: GALAXY" : "MAP: NEBULA");
        });
    mapItem->setColor(Color3B::MAGENTA);
    mapItem->setPosition(visibleSize.width / 2, visibleSize.height * 0.45);

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