#include "Carrot.h"
#include "time.h"
#include "MapUtil.h"
#include "MonsterBase.h"
#include "MonsterManager.h"
#include "GameManager.h"
#include "SoundUtil.h"
#include "SimpleAudioEngine.h"

Carrot::~Carrot()
{
	NOTIFY->removeAllObservers(this);
}

bool Carrot::init()
{
	bool bret = false;

	do
	{
		CC_BREAK_IF(!Entity::init(18, MONSTERCSVFILE));

		iHp = 10;

		GameManager::getInstance()->setICarrotHp(iHp);
		auto tPoints = MapUtil::getInstance()->getMovePoses();
		getSprite()->setAnchorPoint(Vec2(0.5, 0));
		getSprite()->setScale(1.2f);
		this->setPosition((*(tPoints.end() - 2)).x + getContentSize().width / 2 - 20, (*(tPoints.end() - 2)).y - getContentSize().height / 2);
		_HpSprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("BossHP10.png"));
		_HpSprite->setPosition(*(tPoints.end() - 1) - *(tPoints.end() - 2) + Vec2(-20, 10));
		addChild(_HpSprite);
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("ikun/Music/niganma.mp3");
		auto repeatForever = RepeatForever::create(createAction("action1_%02d.png", 30, 1, 0.05));
		getSprite()->runAction(repeatForever);

		NOTIFY->addObserver(this, callfuncO_selector(Carrot::CarrotBeHurt), "CarrotHurt", nullptr);
		registerCarrotTouchEvent();

		srand(time(0));
		//schedule(schedule_selector(Carrot::doAction), 6);
		
		bret = true;
	} while (0);

	return  bret;
}

void Carrot::registerCarrotTouchEvent()
{
	auto pTouchEvent = EventListenerTouchOneByOne::create();
	isClickable = true;
	pTouchEvent->onTouchBegan = [=](Touch * t, Event * e)
	{
		if (getBoundingBox().containsPoint(t->getLocation())&&isClickable) {
			SoundUtil::getInstance()->playEffect("ikun/Music/jntm.mp3");
			isClickable = false;
			auto pCarrot = getSprite();
			pCarrot->stopAllActions();
			pCarrot->setRotation(0);
			pCarrot->setPositionX(pCarrot->getPositionX() - pCarrot->getContentSize().width/4);
			//pCarrot->setTexture(SpriteFrameCache::getInstance()->getSpriteFrameByName(_sModelName + PHOTOPOSTFIX)->getTexture());
			//pCarrot->runAction(doShakeOrBlink(10, 9));createAction("action%d.png", 6, 5)
			auto action = Sequence::create(createAction("action%d.png", 6, 5, 0.1), CallFunc::create([&]() {
				auto repeatForever = RepeatForever::create(createAction("action1_%02d.png", 30, 1, 0.05));
				getSprite()->setPositionX(getSprite()->getPositionX() + getSprite()->getContentSize().width/4);
				isClickable = true;
				getSprite()->runAction(repeatForever);
			}),nullptr);
			pCarrot->runAction(action);
			//pCarrot->runAction(createAction("action%d.png", 6, 5));
		}
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(pTouchEvent, getSprite());
}


//ActionInterval * Carrot::doShakeOrBlink(int iImageFirstNumber, int iImageCount)
//{
//	auto cache = SpriteFrameCache::getInstance();
//	Animation * pAnimation = Animation::create();
//	std::string sSpriteFrameName;
//	for (int i = iImageFirstNumber; i < iImageFirstNumber + iImageCount; i++)
//	{
//		//std::string sSpriteFrameName = _sName + StringUtils::format("%2d.png", i);
//		pAnimation->addSpriteFrame(cache->getSpriteFrameByName(StringUtils::format("action1_%02d.png", i)));
//	}
//	pAnimation->addSpriteFrame(cache->getSpriteFrameByName(_sModelName + PHOTOPOSTFIX));
//	pAnimation->setDelayPerUnit(0.1);
//	pAnimation->setLoops(1);
//	Animate * pAnimate = Animate::create(pAnimation);
//	return pAnimate;
//}

ActionInterval * Carrot::createAction(char* fileName, int iImageCount,int loop,double delay)
{
	auto cache = SpriteFrameCache::getInstance();
	Animation * pAnimation = Animation::create();
	std::string sSpriteFrameName;
	for (int i = 0; i <= iImageCount; i++)
	{
		pAnimation->addSpriteFrame(cache->getSpriteFrameByName(StringUtils::format(fileName, i)));
	}
	pAnimation->setDelayPerUnit(delay);
	pAnimation->setLoops(loop);
	Animate * pAnimate = Animate::create(pAnimation);
	return pAnimate;
}

void Carrot::doAction(float t)
{
	getSprite()->stopAllActions();
	int tRandom = rand() % 3;
	switch (tRandom)
	{
	case 0: getSprite()->runAction(createAction("action1_%02d.png", 30, 1, 0.05));
		break;
	case 1: getSprite()->runAction(createAction("love%02d.png", 52, 1, 0.05));
		break;
	//case 2: getSprite()->runAction(Sequence::create(RotateBy::create(0.2, -20), RotateBy::create(0.2, 20), RotateBy::create(0.2, 30), RotateBy::create(0.2, -30), NULL));
		break;
	default:
		break;
	}
	//SoundUtil::getInstance()->playEffect(StringUtils::format(CARROT, tRandom + 1));
}

void Carrot::CarrotBeHurt(Ref * pRef)
{
	//SoundUtil::getInstance()->playEffect(CARROTCRASH);
	SoundUtil::getInstance()->playEffect("ikun/Music/a.mp3");
	MonsterBase * pMonster = (MonsterBase *)pRef;
	iHp -= pMonster->getIAtk();
	if (iHp <= 0)
	{
		GameManager::getInstance()->setICarrotHp(0);
		auto tGameEndType = en_GameLose;
		NOTIFY->postNotification("gameEnd", reinterpret_cast<Ref*>(&tGameEndType));
		NOTIFY->removeAllObservers(this);
		return;
	}
	GameManager::getInstance()->setICarrotHp(iHp);
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(getSprite());
	unscheduleAllSelectors();
	_HpSprite->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("BossHP%02d.png", iHp)));
	/*std::string HurtSpriteName;
	if (iHp == 5 || iHp == 7)
	{
		HurtSpriteName = StringUtils::format("hlb%d.png", iHp + 1);
	}
	else
	{
		HurtSpriteName = StringUtils::format("hlb%d.png", iHp);
	}*/
	getSprite()->stopAllActions();
	//getSprite()->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(HurtSpriteName));
	getSprite()->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("loose%02d.png", 45-iHp*5)));
}

Rect Carrot::getBoundingBox()const
{
	return Rect(getPositionX() - getContentSize().width / 2, getPositionY(), getContentSize().width, getContentSize().height);
}