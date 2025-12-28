#ifndef Enemy_H
#define Enemy_H
#include "BaseEntity.h"
#include "cocos2d.h"

USING_NS_CC;

class Enemy : public BaseEntity {
public:
  // 工厂方法，传入图片和血量
  static Enemy *create(const std::string &imgPath, int hp);
  void boom();
  virtual void startMove(float duration, float endY);
  // 受伤处理
  void hurt();

protected:
  virtual bool init(const std::string &imgPath, int hp);
};

#endif // Enemy_H