//
//  VictimBase.cpp
//  CarrotFantasy
//
//  Created by Yan on 14-9-15.
//  修改于2023.8.2 by lzy
//

#include "VictimEntityBase.h"
#include "HpSlot.h"
#include "TowerManager.h"
#include "TowerBase.h"
#include "SoundUtil.h"
#include "MonsterManager.h"


VictimEntityBase::~VictimEntityBase()
{
    CC_SAFE_RELEASE_NULL(_pLockAtkTarget);
    CC_SAFE_RELEASE_NULL(_pHpSlot);
    CC_SAFE_RELEASE_NULL(_pLockFlagAnimate);
}

bool VictimEntityBase::init(const int &rIId, const std::string &rSCsvFileName)
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(!Entity::init(rIId, rSCsvFileName));
		freezeCnt = 0;
        _iHp = CsvUtil::getInstance()->getInt(_iId, en_Hp, rSCsvFileName);
		_iState = en_Normal;
		_fSlowDuration = 0;
		_fStopDuration = 0;
		_fPoisonDuration = 0;
        _Pos = getPosition();
        _Size = getContentSize();
		this->schedule(schedule_selector(VictimEntityBase::checkAtkTarget));
		_ElementSprite = Sprite::createWithSpriteFrameName("clear.png");
		_ElementSprite->setScale(0.5);
		addChild(_ElementSprite,INT_MAX);
		_ElementSprite->setPosition(getPosition()+ Vec2(0,getContentSize().height/2));
		schedule([=](float delta) {
			std::string fileName;
			switch (this->element)
			{
			case Pyro:fileName = "pyro.png"; break;
			case Hydro:fileName = "hydro.png"; break;
			case Anemo:fileName = "anemo.png"; break;
			case Electro:fileName = "electro.png"; break;
			case Dendro:fileName = "dendro.png"; break;
			case Cryo:fileName = "cryo.png"; break;
			case Geo:fileName = "geo.png"; break;
			default:
				fileName = "clear.png"; break;
			}
			_ElementSprite->setSpriteFrame(fileName);
		}, 0.1f, "checkElement");

        bRet = true;
    }
    while (0);
    
    return bRet;
}

//检查元素反应
void VictimEntityBase::checkReaction(const Element e)
{	
	if (this->element == Element::Clear) {
		setElement(e);
	}
	else {
		switch (e)
		{
		case Pyro://火
			switch (this->element)
			{
			case Pyro:break;
			case Hydro:Vaporize(); break;
			case Anemo:Swirl(e); break;
			case Electro:OverLoad(); break;
			case Dendro:Combustion(); break;
			case Cryo:Melt(); break;
			case Geo:Shattered(e); break;
			default:
				break;
			}
			break;
		case Hydro://水
			switch (this->element)
			{
			case Pyro:Vaporize(); break;
			case Hydro:break;
			case Anemo:break;
			case Electro:Electro_Charged(); break;
			case Dendro:break;
			case Cryo:Freeze(); break;
			case Geo:Shattered(e); break;
			default:
				break;
			}
			break;
		case Anemo://风
			switch (this->element)
			{
			case Pyro:Swirl(e); break;
			case Hydro:Swirl(e); break;
			case Anemo:break;
			case Electro:Swirl(e); break;
			case Dendro:break;
			case Cryo:Swirl(e); break;
			case Geo: break;
			default:
				break;
			}
			break;
		case Electro://雷
			switch (this->element)
			{
			case Pyro:OverLoad(); break;
			case Hydro:Electro_Charged(); break;
			case Anemo:Swirl(e); break;
			case Electro:break;
			case Dendro:OverDose(); break;
			case Cryo:Superconduct(); break;
			case Geo:Shattered(e); break;
			default:
				break;
			}
			break;
		case Dendro://草
			switch (this->element)
			{
			case Pyro:Combustion(); break;
			case Hydro:break;
			case Anemo:break;
			case Electro:OverGrow(); break;
			case Dendro:break;
			case Cryo:break;
			case Geo:Shattered(e); break;
			default:
				break;
			}
			break;
		case Cryo://冰
			switch (this->element)
			{
			case Pyro:Melt(); break;
			case Hydro:Freeze(); break;
			case Anemo:Swirl(e); break;
			case Electro:Superconduct(); break;
			case Dendro:break;
			case Cryo:break;
			case Geo:Shattered(e); break;
			default:
				break;
			}
			break;
		case Geo://岩
			switch (this->element)
			{
			case Pyro:Shattered(this->element); break;
			case Hydro:Shattered(this->element); break;
			case Anemo: break;
			case Electro:Shattered(this->element); break;
			case Dendro:Shattered(this->element); break;
			case Cryo:Shattered(this->element); break;
			case Geo: break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void VictimEntityBase::beHurt2(const AtkProperty atk)
{
	//checkReaction(atk._elementType);
	setElement(atk._elementType);
	/*
	 *set hp and state
	 */
	if (this->getIsDead())return;
	_iState |= atk._enAtkState;
	_iBulletStateType = atk._iButtltType;
	switch (atk._enAtkState)
	{
	case en_Slow: _fSlowDuration = atk._iDuration; break;
	case en_Stop: _fStopDuration = atk._iDuration; break;
	case en_Poison: _fPoisonDuration = atk._iDuration; break;
	case en_Burn:_fBurnDuration = atk._iDuration; break;
	default:
		break;
	}
	auto tIHp = getIHp() - atk._iAtk;
	if (tIHp > 0)
	{
		unschedule(schedule_selector(VictimEntityBase::closeHpSlot));
		if (!_bHpSlotExsit)
		{
			hpSlotVisible(!_bHpSlotExsit);
			_bHpSlotExsit = true;
		}
		_pHpSlot->setVisible(true);
		schedule(schedule_selector(VictimEntityBase::closeHpSlot), _iHpCloseTime);
		setIHp(tIHp);
		_pHpSlot->setHp(getIHp());
		behurtAction();
	}
	else if (tIHp <= 0)
	{
		doDead();
	}
}

void VictimEntityBase::beHurt(const AtkProperty tBeHurtValue)
{
	
	atk = tBeHurtValue;
	checkReaction(atk._elementType);
	
    /*
     *set hp and state 
     */
	if (this->getIsDead())return;
	_iState |= atk._enAtkState;
	_iBulletStateType = atk._iButtltType;
	switch (atk._enAtkState)
	{
        case en_Slow: _fSlowDuration = atk._iDuration; break;
        case en_Stop: _fStopDuration = atk._iDuration; break;
        case en_Poison: _fPoisonDuration = atk._iDuration; break;
		case en_Burn:_fBurnDuration = atk._iDuration; break;
	default:
		break;
	}
    auto tIHp = getIHp() - atk._iAtk;
    if(tIHp > 0)
    {
        unschedule(schedule_selector(VictimEntityBase::closeHpSlot));
        if(!_bHpSlotExsit)
        {
            hpSlotVisible(!_bHpSlotExsit);
            _bHpSlotExsit = true;
        }
        _pHpSlot->setVisible(true);
        schedule(schedule_selector(VictimEntityBase::closeHpSlot), _iHpCloseTime);
		setIHp(tIHp);
        _pHpSlot->setHp(getIHp());
        behurtAction();
    }
    else if(tIHp <= 0)
    {
        doDead();
    }
}

void VictimEntityBase::closeHpSlot(float dt)
{
    _pHpSlot->setVisible(false);
}


void VictimEntityBase::showReaction(std::string s,Color3B color)
{

	auto tValue = std::make_tuple(getPosition(), s, color);
	NOTIFY->postNotification("monsterReaction", reinterpret_cast<Ref*>(&tValue));
	//Label* type = Label::create(s, "方正喵呜体", 20);
	//type->setColor(color);
	//type->enableBold();
	//type->enableOutline(Color4B::WHITE);
	//addChild(type,INT_MAX);
	//type->runAction(Sequence::create(MoveBy::create(0.4f, Vec2(0, 60)), DelayTime::create(0.4), FadeOut::create(0.4f), CallFunc::create([=]() {type->removeFromParent(); }), NULL));
}

void VictimEntityBase::Vaporize()
{
	if (this->element == Element::Pyro) {
		atk._iAtk *= 2;
	}
	else {
		atk._iAtk *= 1.5;
	}

	showReaction("蒸发", Color3B::ORANGE);
	setElement(Element::Clear);
}

void VictimEntityBase::Melt()
{
	if (this->element == Element::Cryo) {
		atk._iAtk *= 2;
	}
	else {
		atk._iAtk *= 1.5;
	}

	showReaction("融化",Color3B::ORANGE);
	setElement(Element::Clear);
}

void VictimEntityBase::OverGrow()
{
	atk._iAtk *= 1.5;

	showReaction("蔓激化", Color3B::GREEN);
}

void VictimEntityBase::OverDose()
{
	atk._iAtk *= 2;

	showReaction("超激化", Color3B(128, 0, 128));
}

void VictimEntityBase::OverLoad()
{
	Sprite* _doActionSprite = Sprite::create();
	getParent()->addChild(_doActionSprite);
	Animation * pAnimation = Animation::create();
	for (int i = 1; i < 7; i++)
	{
		std::string SpriteFrameName = StringUtils::format("PAnchor-1%d.png", i);
		pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
	}
	pAnimation->setDelayPerUnit(0.05);
	pAnimation->setLoops(1);
	CallFunc * clearCF = CallFunc::create([=]() { _doActionSprite->removeFromParentAndCleanup(true); });
	_doActionSprite->setPosition(getPosition());
	_doActionSprite->runAction(Sequence::create(Animate::create(pAnimation), clearCF, NULL));
	auto tValue = std::make_tuple(this);
	NOTIFY->postNotification("OverLoad", reinterpret_cast<Ref*>(&tValue));
	showReaction("超载", Color3B::MAGENTA);
	setElement(Element::Clear);

}

void VictimEntityBase::Superconduct()
{
	Sprite* _doActionSprite = Sprite::create();
	getParent()->addChild(_doActionSprite);
	Animation * pAnimation = Animation::create();
	for (int i = 1; i < 7; i++)
	{
		std::string SpriteFrameName = StringUtils::format("PBStar-1%d.png", i);
		pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
	}
	pAnimation->setDelayPerUnit(0.05);
	pAnimation->setLoops(1);
	CallFunc * clearCF = CallFunc::create([=]() { _doActionSprite->removeFromParentAndCleanup(true); });
	_doActionSprite->setPosition(getPosition());
	_doActionSprite->runAction(Sequence::create(Animate::create(pAnimation), clearCF, NULL));
	auto tValue = std::make_tuple(this);
	NOTIFY->postNotification("Superconduct", reinterpret_cast<Ref*>(&tValue));
	showReaction("超导", Color3B(128, 0, 128));
	setElement(Element::Clear);
}

void VictimEntityBase::Electro_Charged()
{
	setElement(Element::Clear);
	auto tValue = std::make_tuple(this);
	NOTIFY->postNotification("Electro_Charged", reinterpret_cast<Ref*>(&tValue));
	showReaction("感电", Color3B(128,0,128));
}

void VictimEntityBase::Freeze()
{
	freezeCnt++;
	showReaction("冻结", Color3B::BLUE);
	atk._enAtkState = AtkState::en_Stop;
	atk._iDuration = 4;
	atk._iButtltType = 15;
	setElement(Element::Clear); 
}

void VictimEntityBase::Shattered(const Element e)
{
	setElement(e);
	Sprite* _doActionSprite = Sprite::create();
	getParent()->addChild(_doActionSprite);
	Animation * pAnimation = Animation::create();
	for (int i = 1; i < 7; i++)
	{
		std::string SpriteFrameName = StringUtils::format("PStar-1%d.png", i);
		pAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(SpriteFrameName));
	}
	pAnimation->setDelayPerUnit(0.05);
	pAnimation->setLoops(1);
	CallFunc * clearCF = CallFunc::create([=]() { _doActionSprite->removeFromParentAndCleanup(true); });
	_doActionSprite->setPosition(getPosition());
	_doActionSprite->runAction(Sequence::create(Animate::create(pAnimation), clearCF, NULL));
	auto tValue = std::make_tuple(e, this);
	NOTIFY->postNotification("Shattered", reinterpret_cast<Ref*>(&tValue));
	showReaction("结晶", Color3B::YELLOW);
	
}

void VictimEntityBase::Combustion()
{
	showReaction("燃烧", Color3B::ORANGE);
	AtkProperty burn;
	burn._enAtkState = AtkState::en_Burn;
	burn._iDuration = 2;
	burn._iAtk = 0;
	burn._elementType = Element::Pyro;
	setElement(Element::Clear);
	beHurt2(burn);
}

void VictimEntityBase::Swirl(const Element e)
{
	setElement(Element::Clear);
	auto tValue = std::make_tuple(element, e, this);
	NOTIFY->postNotification("affectOthers", reinterpret_cast<Ref*>(&tValue));
	showReaction("扩散", Color3B(57, 255, 197));

}


void VictimEntityBase::deadAction(const std::string &rSDeadImageFile)
{
    //auto tValue = std::make_tuple(getPosition(), _iValue);
    //NOTIFY->postNotification("monsterDeadMoney", reinterpret_cast<Ref*>(&tValue));

    if(_pHpSlot) _pHpSlot->removeFromParent();
    if(_pLockAtkTarget) _pLockAtkTarget->removeFromParent();
    Entity::deadAction();
}

void VictimEntityBase::createHpSlotSprite()
{
    _pHpSlot = HpSlot::create(this);
    _pHpSlot->retain();
    auto tPos = getPosition();
    auto tSpriteSize = getContentSize();
    auto tSlotSize = _pHpSlot->getContentSize();
    _pHpSlot->setPosition(Vec2(-tSlotSize.width / 2 - 3, tSpriteSize.height / 2 - 12));
    addChild(_pHpSlot);
}

void VictimEntityBase::createLockAtkSprite()
{
	createLockFlagAnimate();
	_pLockAtkTarget = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("point01.png"));
    CC_SAFE_RETAIN(_pLockAtkTarget);
	_pLockAtkTarget->setPosition(Vec2(0, _Pos.y + _Size.height / 2 + 12));
	addChild(_pLockAtkTarget, 2);
}

void VictimEntityBase::createLockFlagAnimate()
{
    auto pAnimation = Animation::create();
    auto pSpriteFrameCache = SpriteFrameCache::getInstance();
    for(int i = 1; i <= 3; ++i)
        pAnimation->addSpriteFrame(pSpriteFrameCache->getSpriteFrameByName(StringUtils::format("point0%d.png", i)));
    pAnimation->setLoops(-1);
    pAnimation->setDelayPerUnit(0.1f);
    _pLockFlagAnimate = Animate::create(pAnimation);
    _pLockFlagAnimate->retain();
}

void VictimEntityBase::setAtkTarget(const bool &rBisAtkTarget)
{
    _bIsAtkTarget = rBisAtkTarget;
    if(_bIsAtkTarget)
    {
		if (!_pLockAtkTarget) createLockAtkSprite();
        _pLockAtkTarget->runAction(_pLockFlagAnimate);
        _pLockAtkTarget->setVisible(_bIsAtkTarget);
		SoundUtil::getInstance()->playEffect(SELECTVICTIMENTITYBASE);
    }
    else
    {
        if(!_pLockAtkTarget) return;
        _pLockAtkTarget->stopAllActions();
        _pLockAtkTarget->setVisible(_bIsAtkTarget);
    }
}

void VictimEntityBase::hpSlotVisible(const bool &rBIsVisible)
{
    if(rBIsVisible) createHpSlotSprite();
    else _pHpSlot->setVisible(rBIsVisible);
}

void VictimEntityBase::checkAtkTarget(float dt){
    auto tTowerVec = TowerManager::getInstance()->getTowerVec();
	if (_bIsAtkTarget){
		for (auto & towerItem : tTowerVec)
        {
			if (towerItem->isInAtkRange(this->getPosition())){
				towerItem->setAtkTarget(this);
                towerItem->setIsHaveAtkTarget(true);
			}
		}
	}
}



