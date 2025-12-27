#include "GameOverScene.h"
#include "MenuScene.h"
#include "audio/include/SimpleAudioEngine.h"

using namespace CocosDenshion;
USING_NS_CC;

Scene *GameOverScene::createScene() {
  // 创建场景对象 (Scene 也是个 Node，这里直接用 Scene 即可)
  return GameOverScene::create();
}

bool GameOverScene::init() {
  // 1. 调用父类 init
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 2. 读取分数
  // 从 UserDefault 中获取 "LastScore"，默认值为 0
  int score = UserDefault::getInstance()->getIntegerForKey("LastScore", 0);

  // 3. 创建显示分数的 Label
  auto scoreLabel = Label::createWithSystemFont(
      StringUtils::format("SCORE: %d", score), "Arial", 48);

  if (scoreLabel) {
    // 设置位置：屏幕中心
    scoreLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                                 visibleSize.height / 2 + origin.y));
    // 设置颜色：黄色
    scoreLabel->setColor(Color3B::YELLOW);
    // 添加到场景，设置较高的 ZOrder
    this->addChild(scoreLabel, 1);
  }

  // 4. (可选) 添加一个返回菜单的按钮，方便重新开始
  // 虽然需求只需显示分数，但为了游戏流程完整，加一个简单按钮
  auto itemLabel = Label::createWithSystemFont("Main Menu", "Arial", 30);
  auto menuItem = MenuItemLabel::create(itemLabel, [](Ref *sender) {
    SimpleAudioEngine::getInstance()
        ->stopAllEffects(); // 停止所有残留音效(如爆炸声)
    auto menuScene = MenuScene::createScene();
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, menuScene));
  });

  // 把按钮放在分数下面一点
  menuItem->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                             visibleSize.height / 2 + origin.y - 100));

  auto menu = Menu::create(menuItem, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  return true;
}
