//
//  TowerPlane.h
//  CarrotFantasy
//
//  Created by 王紫叶 on 14-10-3.
//
//

#ifndef __CarrotFantasy__TowerPlane__
#define __CarrotFantasy__TowerPlane__

#include "TowerBase.h"
class TowerPlane :public TowerBase{
    
	int const rId = 34;
    
	virtual bool init();
    
	virtual void fire(float dt);
    
    Sprite * _doAnimationSprite;
    
public:
    void attack();
	bool checkLineRectIntersection(const Vec2 & startPos, const Vec2 & endPos, const Rect & rect);
	bool checkLineIntersection(const Vec2 & p1, const Vec2 & p2, const Vec2 & p3, const Vec2 & p4);
	CREATE_FUNC(TowerPlane);
};

#endif /* defined(__CarrotFantasy__TowerPlane__) */
