//
//  MonsterLayer.cpp
//  CarrotFantasy
//
//  Created by Yan on 14-9-13.
//
//

#include "MonsterLayer.h"
#include "MapUtil.h"

bool MonsterLayer::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!Layer::init());

		auto pStart = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("start01.png"));
		pStart->setPosition(MapUtil::getInstance()->getMovePoses().at(0) + Vec2(40, -15));
		addChild(pStart);

        registerMonsterDeadEvent();
		registerMonsterReactionEvent();
        bRet = true;
    }
    while (0);
    
    return bRet;
}


void MonsterLayer::registerMonsterDeadEvent()
{
    NOTIFY->addObserver(this, callfuncO_selector(MonsterLayer::monsterDead), "monsterDeadMoney", nullptr);
}

void MonsterLayer::registerMonsterReactionEvent()
{
	NOTIFY->addObserver(this, callfuncO_selector(MonsterLayer::monsterReaction), "monsterReaction", nullptr);
	NOTIFY->addObserver(this, callfuncO_selector(MonsterLayer::drawElectro), "drawElectro", nullptr);
}

void MonsterLayer::monsterDead(Ref *pData)
{
    auto tValue = *(reinterpret_cast<std::tuple<Vec2, int>*>(pData));
    auto tMoney = std::get<1>(tValue);
    auto pMoney = Sprite::createWithSpriteFrameName(StringUtils::format("money%d.png", tMoney));
    auto pVec2 = std::get<0>(tValue);
    pMoney->setPosition(pVec2);
    addChild(pMoney);
    pMoney->runAction(Sequence::create(MoveBy::create(0.6f, Vec2(0, 100)), DelayTime::create(0.4), FadeOut::create(0.4f), CallFunc::create([=](){pMoney->removeFromParent();}) ,  NULL));

}

void MonsterLayer::monsterReaction(Ref *pData)
{
	auto tValue = *(reinterpret_cast<std::tuple<Vec2, std::string,Color3B>*>(pData));
	auto tContent = std::get<1>(tValue);
	auto pVec2 = std::get<0>(tValue);
	auto tColor = std::get<2>(tValue);
	Label* type = Label::createWithTTF(tContent, "fonts/zh-cn.ttf", 30);
	type->setPosition(pVec2);
	type->setColor(tColor);
	type->enableOutline(Color4B::BLACK,1);
	addChild(type, INT_MAX);
	auto spawn = Spawn::create(ScaleTo::create(0.4f, 0.7f), MoveBy::create(0.4f, Vec2(0, 30)), nullptr);
	type->runAction(Sequence::create(ScaleTo::create(0.2f, 1.2f), spawn,DelayTime::create(0.4), FadeOut::create(0.4f), CallFunc::create([=]() {type->removeFromParent(); }), NULL));

}

void MonsterLayer::drawElectro(Ref *pData)
{
	auto tValue = *(reinterpret_cast<std::tuple<Vec2, Vec2>*>(pData));
	auto p1 = std::get<0>(tValue);
	auto p2 = std::get<1>(tValue);
	
	auto _doActionSprite = Sprite::create();
	addChild(_doActionSprite, 100);

	Animation * pAnimation = Animation::create();
	for (int i = 1; i <= 4; i++) {
		std::string SpriteFrameName = "PBall1" + StringUtils::format("%d.png", i);
		pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
	}
	pAnimation->setDelayPerUnit(0.03);
	pAnimation->setLoops(2);

	CallFunc * clearCF = CallFunc::create([=]() { _doActionSprite->removeFromParentAndCleanup(true); });

	_doActionSprite->setVisible(true);
	//_doActionSprite->setScale(p1.distance(p2) / _doActionSprite->getContentSize().height);
	_doActionSprite->setScale(0.5);
	_doActionSprite->setPosition((p1 + p2) / 2);
	_doActionSprite->setRotation(90 - 180 * (p2 - p1).getAngle() / M_PI);
	_doActionSprite->runAction(Sequence::create(Animate::create(pAnimation),clearCF,NULL));

}


void MonsterLayer::onExit()
{
    Layer::onExit();
    NOTIFY->removeAllObservers(this);
}

