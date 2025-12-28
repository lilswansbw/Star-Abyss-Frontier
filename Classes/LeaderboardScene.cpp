#include "LeaderboardScene.h"
#include "MenuScene.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

// 默认数据
static const char *DEFAULT_NAMES[5] = {"ACE", "NOVA", "STAR", "HAWK", "ZERO"};
static const int DEFAULT_SCORES[5] = {5000, 4000, 3000, 2000, 1000};

Scene *LeaderboardScene::createScene() { return LeaderboardScene::create(); }

// ==========================================
// 获取当前高分榜
// ==========================================
std::vector<LeaderboardEntry> LeaderboardScene::getHighScores() {
  std::vector<LeaderboardEntry> entries;
  auto ud = UserDefault::getInstance();

  for (int i = 0; i < MAX_SCORES; i++) {
    LeaderboardEntry entry;

    std::string scoreKey = StringUtils::format("HighScore%d", i + 1);
    std::string nameKey = StringUtils::format("HighName%d", i + 1);

    entry.score = ud->getIntegerForKey(scoreKey.c_str(), DEFAULT_SCORES[i]);
    entry.name = ud->getStringForKey(nameKey.c_str(), DEFAULT_NAMES[i]);

    entries.push_back(entry);
  }

  return entries;
}

// ==========================================
// 更新高分榜（插入排序）
// ==========================================
void LeaderboardScene::updateHighScores(int currentScore,
                                        const std::string &playerName) {
  auto ud = UserDefault::getInstance();

  // 1. 读取现有数据
  std::vector<LeaderboardEntry> entries = getHighScores();

  // 2. 添加新条目
  LeaderboardEntry newEntry;
  newEntry.score = currentScore;
  newEntry.name = playerName.empty() ? "PILOT" : playerName;
  entries.push_back(newEntry);

  // 3. 按分数降序排序
  std::sort(entries.begin(), entries.end(),
            [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
              return a.score > b.score;
            });

  // 4. 只保留前5名
  if (entries.size() > MAX_SCORES) {
    entries.resize(MAX_SCORES);
  }

  // 5. 写回 UserDefault
  for (int i = 0; i < MAX_SCORES; i++) {
    std::string scoreKey = StringUtils::format("HighScore%d", i + 1);
    std::string nameKey = StringUtils::format("HighName%d", i + 1);

    ud->setIntegerForKey(scoreKey.c_str(), entries[i].score);
    ud->setStringForKey(nameKey.c_str(), entries[i].name);
  }
  ud->flush();
}

// ==========================================
// 排行榜场景初始化
// ==========================================
bool LeaderboardScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  float centerX = origin.x + visibleSize.width / 2;
  float centerY = origin.y + visibleSize.height / 2;

  // ==========================================
  // 背景
  // ==========================================
  auto bg = LayerColor::create(Color4B(20, 20, 40, 255)); // 深蓝色背景
  this->addChild(bg, -1);

  // ==========================================
  // 标题: HALL OF FAME
  // ==========================================
  auto titleLabel = Label::createWithSystemFont("HALL OF FAME", "Arial", 56);
  titleLabel->setPosition(Vec2(centerX, origin.y + visibleSize.height - 80));
  titleLabel->setColor(Color3B(255, 215, 0));              // 金色
  titleLabel->enableOutline(Color4B(139, 69, 19, 255), 2); // 棕色描边
  this->addChild(titleLabel, 1);

  // ==========================================
  // 分数列表
  // ==========================================
  std::vector<LeaderboardEntry> entries = getHighScores();
  float startY = centerY + 100;
  float lineSpacing = 60;

  for (int i = 0; i < MAX_SCORES; i++) {
    // 创建排名标签：显示 "1. NAME - 10000"
    std::string text = StringUtils::format(
        "%d. %s - %d", i + 1, entries[i].name.c_str(), entries[i].score);

    auto scoreLabel = Label::createWithSystemFont(text, "Arial", 36);
    scoreLabel->setPosition(Vec2(centerX, startY - i * lineSpacing));

    // 根据排名设置颜色
    switch (i) {
    case 0:
      scoreLabel->setColor(Color3B(255, 215, 0)); // 金色 - 第1名
      break;
    case 1:
      scoreLabel->setColor(Color3B(192, 192, 192)); // 银色 - 第2名
      break;
    case 2:
      scoreLabel->setColor(Color3B(205, 127, 50)); // 铜色 - 第3名
      break;
    default:
      scoreLabel->setColor(Color3B::WHITE); // 白色 - 其他
      break;
    }

    this->addChild(scoreLabel, 1);
  }

  // ==========================================
  // 返回按钮
  // ==========================================
  auto backLabel = Label::createWithSystemFont("< BACK TO MENU", "Arial", 32);
  auto backItem = MenuItemLabel::create(backLabel, [](Ref *sender) {
    SimpleAudioEngine::getInstance()->stopAllEffects(); // 停止爆炸等残留音效
    auto menuScene = MenuScene::createScene();
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, menuScene));
  });

  backItem->setPosition(Vec2(centerX, origin.y + 80));
  backItem->setColor(Color3B(100, 200, 255)); // 浅蓝色

  auto menu = Menu::create(backItem, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  return true;
}
