#include "TowerSnow.h"
#include "MonsterBase.h"
#include "BarrierBase.h"
#include "MonsterManager.h"
#include "BarrierManager.h"
#include "SoundUtil.h"

bool TowerSnow::init()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!TowerBase::init(rId));
		unschedule(schedule_selector(TowerSnow::doRocation));
		_doAnimationSprite = Sprite::create();
		_doAnimationSprite->setVisible(false);
		addChild(_doAnimationSprite);
		elementType = Element::Cryo;
		bRet = true;
	} while (0);
	return bRet;
}

void TowerSnow::fire(float dt){
	if (!_pAtkTarget || _pAtkTarget->getIsDead()) return;

	_doAnimationSprite->setVisible(true);
	Animation * pAnimation = Animation::create();
	for (int i = 1; i < 6; i++)
	{
		std::string SpriteFrameName = "P" + _sModelName + StringUtils::format("%d.png", i);
		pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
	}
	pAnimation->setDelayPerUnit(0.1);
	pAnimation->setLoops(1);
	SoundUtil::getInstance()->playEffect("Music/Towers/Snow.mp3");

	CallFunc * pCallFunc = CallFunc::create([=](){
		attack();
		_doAnimationSprite->setVisible(false);
	});

	_doAnimationSprite->runAction(Sequence::create(Animate::create(pAnimation), pCallFunc, NULL));
}

void TowerSnow::attack()
{
	AtkProperty atk;
	atk._enAtkState = AtkState::en_Normal;
	atk._iAtk = _iBulletId;
	atk._iDuration = 0;
	atk._iButtltType = _iBulletId;
	atk._elementType = elementType;

	Rect rect;
	if (getIId() == 31){ rect = Rect(this->getPositionX() - 100, this->getPositionY() - 100, 200, 200); }
	if (getIId() == 32){ rect = Rect(this->getPositionX() - 120, this->getPositionY() - 120, 240, 240); }
	if (getIId() == 33){ rect = Rect(this->getPositionX() - 140, this->getPositionY() - 140, 280, 280); }
	
	Vector<MonsterBase *>  MonsterVector = Vector<MonsterBase *>(MonsterManager::getInstance()->getMonsterVec());
	for (auto mIter = MonsterVector.begin(); mIter != MonsterVector.end();)
	{
		MonsterBase * pMonster = (MonsterBase *)(*mIter);
		if (rect.intersectsRect(pMonster->getBoundingBox()))
		{
			pMonster->beHurt(atk);
		}
		if (pMonster->getIHp() <= 0 || pMonster->getIsDead())
		{
			///*mIter =*/ MonsterVector.eraseObject(pMonster);
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
		{
			pBarrier->beHurt(atk);
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
