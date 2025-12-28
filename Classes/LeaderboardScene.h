#ifndef __LEADERBOARD_SCENE_H__
#define __LEADERBOARD_SCENE_H__

#include "cocos2d.h"
#include <string>
#include <vector>


// 排行榜条目结构
struct LeaderboardEntry {
  std::string name;
  int score;
};

class LeaderboardScene : public cocos2d::Scene {
public:
  static cocos2d::Scene *createScene();
  virtual bool init();
  CREATE_FUNC(LeaderboardScene);

  // 高分榜辅助函数（静态，供其他场景调用）
  static void updateHighScores(int currentScore, const std::string &playerName);
  static std::vector<LeaderboardEntry> getHighScores();

private:
  static const int MAX_SCORES = 5;
};

#endif // __LEADERBOARD_SCENE_H__
