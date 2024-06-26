#ifndef __CarrotFantasy__TowerSun__
#define __CarrotFantasy__TowerSun__
#include "TowerBase.h"
class TowerSun :public TowerBase{

	int const rId = 22;

	virtual bool init();

	virtual void fire(float dt);

	virtual bool containsPoint(Vec2 v, float r, Vec2 v2);

	Sprite * _doAnimationSprite;

	void attack();

public:

	CREATE_FUNC(TowerSun);
};
#endif /* defined(__CarrotFantasy__TowerSun__) */