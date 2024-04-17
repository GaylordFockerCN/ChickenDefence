#include "TowerSun.h"
#include "MonsterBase.h"
#include "BarrierBase.h"
#include "MonsterManager.h"
#include "BarrierManager.h"
#include "SoundUtil.h"

bool TowerSun::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!TowerBase::init(rId));
		unschedule(schedule_selector(TowerSun::doRocation));
		_doAnimationSprite = Sprite::create();
		_doAnimationSprite->setVisible(false);
		addChild(_doAnimationSprite);
		elementType = Element::Pyro;
		bRet = true;
	} while (0);
	return bRet;
}

void TowerSun::fire(float dt){
	if (!_pAtkTarget || _pAtkTarget->getIsDead()) return;

	_pTowerPanel->runAction(Sequence::create(RotateBy::create(0.1, 30), RotateBy::create(0.1, -30), NULL));

	_doAnimationSprite->setVisible(true);
	Animation * pAnimation = Animation::create();
	for (int i = 1; i < 6; i++)
	{
		std::string SpriteFrameName = "P" + _sName + StringUtils::format("%d.png", i);
		pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
	}
	pAnimation->setDelayPerUnit(0.1);
	pAnimation->setLoops(1);
	SoundUtil::getInstance()->playEffect("Music/Towers/Sun.mp3");


	CallFunc * pCallFunc = CallFunc::create([=](){
		attack();
		_doAnimationSprite->setVisible(false);
	});

	_doAnimationSprite->runAction(Sequence::create(Animate::create(pAnimation),pCallFunc,NULL));
}

bool TowerSun::containsPoint(Vec2 c,float r,Vec2 p) {
	return c.distance(p) <= r;
}

void TowerSun::attack()
{
	AtkProperty atk;
	atk._enAtkState = 1;
	atk._iAtk = _iBulletId;
	atk._iDuration = 0;
	atk._elementType = elementType;

	Animation * pHurtAction = Animation::create();
	for (int i = 1; i < 10; i++)
	{
		std::string hurtSpriteName = StringUtils::format("PSun0%d.png",i);
		pHurtAction->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(hurtSpriteName));
	}
	pHurtAction->setDelayPerUnit(0.03);
	pHurtAction->setLoops(1);

	Rect rect;
	if (getIId() == 22){ rect = Rect(this->getPositionX() - 105, this->getPositionY() - 105,210,210); }
	if (getIId() == 23){ rect = Rect(this->getPositionX() - 125, this->getPositionY() - 125,250,250); }
	if (getIId() == 24){ rect = Rect(this->getPositionX() - 145, this->getPositionY() - 145,290,290); }
	/*float r = 0;
	if (getIId() == 22) { r = 120; }
	if (getIId() == 23) { r = 140; }
	if (getIId() == 24) { r = 160; }*/
	Vector<MonsterBase *>  MonsterVector = Vector<MonsterBase *>(MonsterManager::getInstance()->getMonsterVec());
	
	for (auto mIter = MonsterVector.begin(); mIter != MonsterVector.end();)
	{
		MonsterBase * pMonster = (MonsterBase *)(*mIter);
		
		//if (containsPoint(getPosition(),r,pMonster->getPosition()))
		if (rect.intersectsRect(pMonster->getBoundingBox()))
		{
			pMonster->beHurt(atk);
			Sprite * pMonsterHurt = Sprite::create();
			pMonster->addChild(pMonsterHurt);
			CallFunc * pClear = CallFunc::create([=](){ pMonsterHurt->removeFromParentAndCleanup(true); });
			pMonsterHurt->runAction(Sequence::create(Animate::create(pHurtAction), pClear, NULL));
		}
		if (pMonster->getIHp() <= 0 || pMonster->getIsDead())
		{
			//*mIter =*/ MonsterVector.eraseObject(pMonster);
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
		BarrierBase * pBarrier = (BarrierBase *)(*bIter);
		
		if (rect.intersectsRect(pBarrier->getBoundingBox()))
		//if (containsPoint(getPosition(), r, pBarrier->getPosition()))
		{
			pBarrier->beHurt(atk);
			Sprite * pBarrierHurt = Sprite::create();
			pBarrier->addChild(pBarrierHurt);
			CallFunc * pClear = CallFunc::create([=](){ pBarrierHurt->removeFromParentAndCleanup(true); });
			pBarrierHurt->runAction(Sequence::create(Animate::create(pHurtAction), pClear, NULL));
		}
		if (pBarrier->getIHp() <= 0 || pBarrier->getIsDead())
		{
			///*bIter =*/ BarrierVector.eraseObject(pBarrier);
			bIter = BarrierVector.erase(bIter);
		}
		else
		{
			++bIter;
		}
		
	}
}


