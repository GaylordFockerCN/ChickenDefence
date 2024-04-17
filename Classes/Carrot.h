

#ifndef __CarrotFantasy__Carrot__
#define __CarrotFantasy__Carrot__

#include "Entity.h"

class Carrot :public Entity
{
    
public:
    
	CREATE_FUNC(Carrot);

	virtual Rect getBoundingBox()const;
    
    virtual ~Carrot();
    
	ActionInterval * createAction(char* fileName, int iImageCount, int loop, double delay);

protected:
    
	virtual bool init();
	/**
	 * shake or blink action
	 */
	ActionInterval * doShakeOrBlink(int iImageFirstNumber, int iImageCount);
	
	/**
	 * action update
	 */
	virtual void doAction(float t);

	virtual void CarrotBeHurt(Ref * pRef);
    
    virtual void registerCarrotTouchEvent();

	Sprite * _HpSprite;

	CC_SYNTHESIZE_PASS_BY_REF(int, iHp, IHP);

	CC_SYNTHESIZE_PASS_BY_REF(bool, isClickable, IsClickable);

};

#endif /* defined(__CarrotFantasy__Carrot__) */