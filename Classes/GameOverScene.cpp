#include "GameOverScene.h"
#include "LeaderboardScene.h"
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
  float centerX = origin.x + visibleSize.width / 2;
  float centerY = origin.y + visibleSize.height / 2;

  // 2. 读取分数和玩家名字
  int score = UserDefault::getInstance()->getIntegerForKey("LastScore", 0);
  std::string playerName =
      UserDefault::getInstance()->getStringForKey("PlayerName", "PILOT");

  // ==========================================
  // [排行榜] 更新高分记录（包含玩家名字）
  // ==========================================
  LeaderboardScene::updateHighScores(score, playerName);

  // 3. 背景
  auto bg = LayerColor::create(Color4B(30, 30, 50, 255));
  this->addChild(bg, -1);

  // 4. GAME OVER 标题
  auto titleLabel = Label::createWithSystemFont("GAME OVER", "Arial", 64);
  titleLabel->setPosition(Vec2(centerX, centerY + 120));
  titleLabel->setColor(Color3B(255, 80, 80)); // 红色
  this->addChild(titleLabel, 1);

  // 5. 创建显示分数的 Label
  auto scoreLabel = Label::createWithSystemFont(
      StringUtils::format("SCORE: %d", score), "Arial", 48);

  if (scoreLabel) {
    // 设置位置：屏幕中心
    scoreLabel->setPosition(Vec2(centerX, centerY + 30));
    // 设置颜色：黄色
    scoreLabel->setColor(Color3B::YELLOW);
    // 添加到场景，设置较高的 ZOrder
    this->addChild(scoreLabel, 1);
  }

  // ==========================================
  // 6. 按钮区域
  // ==========================================

  // [返回菜单按钮]
  auto menuLabel = Label::createWithSystemFont("MAIN MENU", "Arial", 30);
  auto menuItem = MenuItemLabel::create(menuLabel, [](Ref *sender) {
    SimpleAudioEngine::getInstance()->stopAllEffects();
    auto menuScene = MenuScene::createScene();
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, menuScene));
  });
  menuItem->setPosition(Vec2(centerX, centerY - 60));

  // [查看排行榜按钮]
  auto leaderboardLabel =
      Label::createWithSystemFont("VIEW LEADERBOARD", "Arial", 30);
  auto leaderboardItem =
      MenuItemLabel::create(leaderboardLabel, [](Ref *sender) {
        auto leaderboardScene = LeaderboardScene::createScene();
        Director::getInstance()->replaceScene(
            TransitionFade::create(0.5f, leaderboardScene));
      });
  leaderboardItem->setPosition(Vec2(centerX, centerY - 120));
  leaderboardItem->setColor(Color3B(100, 200, 255)); // 浅蓝色

  auto menu = Menu::create(menuItem, leaderboardItem, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  return true;
}
