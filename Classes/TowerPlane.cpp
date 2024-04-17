//
//  TowerPlane.cpp
//  CarrotFantasy
//
//  Created by 王紫叶 on 14-10-3.
//
//

#include "TowerPlane.h"
#include "VictimEntityBase.h"
#include "MonsterBase.h"
#include "BarrierBase.h"
#include "MonsterManager.h"
#include "BarrierManager.h"
#include "SoundUtil.h"

bool TowerPlane::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!TowerBase::init(rId));
        _doAnimationSprite = Sprite::create();
        _doAnimationSprite->setAnchorPoint(Vec2(0.5,0));
		_doAnimationSprite->setContentSize(Size(10,1290));
        _doAnimationSprite->setVisible(false);
		addChild(_doAnimationSprite);
		elementType = Element::Electro;
		bRet = true;
	} while (0);
	return bRet;
}
void TowerPlane::fire(float dt){  
	if (!_pAtkTarget || _pAtkTarget->getIsDead()) return;
    
	Animation *pAnimation = Animation::create(); 
    for (int i = 1; i <= 3; i++) {
        
        std::string SpriteFrameName = _sModelName + StringUtils::format("%d.png",i);
        
        pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
    }
    pAnimation->setDelayPerUnit(0.1f);
    pAnimation->setLoops(1);
	SoundUtil::getInstance()->playEffect("Music/Towers/Plane.mp3");

    CallFunc *pCallFunc = CallFunc::create([=](){
        
        attack();
    });
    getSprite()->runAction(Sequence::create(Animate::create(pAnimation),pCallFunc,NULL));
}
void TowerPlane::attack(){
    
    AtkProperty atk;
    atk._enAtkState = 1;
    atk._iAtk = _iBulletId;
    atk._iDuration = 0;
	atk._elementType = elementType;

	_doAnimationSprite->setVisible(true);

	_doAnimationSprite->setRotation(getSprite()->getRotation());

    Animation *pAnimation = Animation::create();
    for (int i = 1; i <= 3; i++) {
        std::string SpriteFrameName = _sName + StringUtils::format("%d.png",i);
        pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
    }
    pAnimation->setLoops(1);
    pAnimation->setDelayPerUnit(0.1f);
    CallFunc *pCallFunc = CallFunc::create([=](){
        _doAnimationSprite->setVisible(false);
    });
    
	_doAnimationSprite->runAction(Sequence::create(Animate::create(pAnimation),pCallFunc,NULL));

    Animation *pHurtAnimation = Animation::create();
    for (int i = 1; i <= 5; i++) {
        std::string SpriteFrameName = StringUtils::format("PPlane0%d.png",i);
        pHurtAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
    }

    pHurtAnimation->setLoops(1);
    pHurtAnimation->setDelayPerUnit(0.1f);
	
	/*int rt;
	int rd;
	if (_pAtkTarget && _pAtkTarget->getPositionX() > this->getPositionX())
	{
		rt = int(getSprite()->getRotation() + 365) % 360;
		rd = int(getSprite()->getRotation() + 355) % 360;
	}
	else
	{
		rt = int(getSprite()->getRotation() + 5) % 360;
		rd = int(getSprite()->getRotation() - 5) % 360;
	}*/

	float Hypotenuse = Vec2::ZERO.distance(Director::getInstance()->getVisibleSize());
	Vec2 MovetoPosition = Vec2(Hypotenuse * sin(getSprite()->getRotation() * M_PI / 180), Hypotenuse * cos(getSprite()->getRotation() * M_PI / 180));

	Vector<MonsterBase *> MonsterVector = Vector<MonsterBase *>(MonsterManager::getInstance()->getMonsterVec());
    for (auto mIter = MonsterVector.begin(); mIter != MonsterVector.end();)
    {
        MonsterBase *pMonster = dynamic_cast<MonsterBase *>(*mIter);
		Rect box = pMonster->getBoundingBox();
		/*int rM = int(90 - 180 * (pMonster->getPosition() - this->getPosition()).getAngle() / M_PI) % 360;
        if (rM >= rd && rM <= rt) {*/
		if(checkLineRectIntersection(getPosition(),MovetoPosition,box)){
            pMonster->beHurt(atk);
			Sprite * pMonsterHurt = Sprite::create();
			pMonster->addChild(pMonsterHurt);
			CallFunc * pClear = CallFunc::create([=](){ pMonsterHurt->removeFromParentAndCleanup(true); });
			pMonsterHurt->runAction(Sequence::create(Animate::create(pHurtAnimation), pClear, NULL));
        }
		if (pMonster->getIHp() <= 0 || pMonster->getIsDead())
		{
			///*mIter = (auto)*/MonsterVector.eraseObject(pMonster);
			mIter = MonsterVector.erase(mIter);
		}
		else
		{
			++mIter;
		}
    }
	
	Vector<BarrierBase *> BarrierVector = Vector<BarrierBase *>(BarrierManager::getInstance()->getBarrierVec());
    for (auto bIter = BarrierVector.begin(); bIter != BarrierVector.end();)
    {
        BarrierBase *pBarrier = dynamic_cast<BarrierBase *>(*bIter);
		Rect box = pBarrier->getBoundingBox();
		//int rB = int(90 - 180 * (pBarrier->getPosition() - this->getPosition()).getAngle() / M_PI) % 360;
		//if (rB >= rd && rB <= rt) {
		if (checkLineRectIntersection(getPosition(), MovetoPosition, box)) {
			pBarrier->beHurt(atk);
			Sprite * pBarrierHurt = Sprite::create();
			pBarrier->addChild(pBarrierHurt);
			CallFunc * pClear = CallFunc::create([=](){ pBarrierHurt->removeFromParentAndCleanup(true); });
			pBarrierHurt->runAction(Sequence::create(Animate::create(pHurtAnimation), pClear, NULL));
        }
		if (pBarrier->getIHp() <= 0 || pBarrier->getIsDead())
		{
			///*bIter = */BarrierVector.eraseObject(pBarrier);
			bIter = BarrierVector.erase(bIter);
		}
		else
		{
			++bIter;
		}
    }
}

bool TowerPlane::checkLineRectIntersection(const Vec2& startPos, const Vec2& endPos, const Rect& rect) {
	// 获取矩形的四个角点
	Vec2 rectTopLeft(rect.getMinX(), rect.getMaxY());
	Vec2 rectTopRight(rect.getMaxX(), rect.getMaxY());
	Vec2 rectBottomRight(rect.getMaxX(), rect.getMinY());
	Vec2 rectBottomLeft(rect.getMinX(), rect.getMinY());

	// 检查线段与矩形的四条边是否有交点
	if (checkLineIntersection(startPos, endPos, rectTopLeft, rectTopRight)) return true;
	if (checkLineIntersection(startPos, endPos, rectTopRight, rectBottomRight)) return true;
	if (checkLineIntersection(startPos, endPos, rectBottomRight, rectBottomLeft)) return true;
	if (checkLineIntersection(startPos, endPos, rectBottomLeft, rectTopLeft)) return true;

	// 线段与矩形无交点
	return false;
}

bool TowerPlane::checkLineIntersection(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& p4) {
	// 计算线段的方向向量
	Vec2 dir1 = p2 - p1;
	Vec2 dir2 = p4 - p3;

	// 利用叉积计算线段的交点位置
	float cross = dir1.cross(dir2);
	if (cross == 0) return false; // 线段平行或共线

	float t1 = (p3 - p1).cross(dir2) / cross;
	float t2 = (p3 - p1).cross(dir1) / cross;

	// 检查交点位置是否在两个线段上
	if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
		return true; // 有交点
	}

	return false; // 无交点
}



