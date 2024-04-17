//
//  MonsterManager.cpp
//  CarrotFantasy
//
//  Created by Yan on 14-9-13.
//
//

#include "MonsterManager.h"
#include "MonsterBase.h"
#include "GameManager.h"

MonsterManager *MonsterManager::_gInstance;

MonsterManager::MonsterManager()
{
}

void MonsterManager::clearBeAtkLockState()
{
    for(auto &iterMonster : *_pMonsterVec)
    {
        iterMonster->setAtkTarget(false);
    }
}

MonsterManager::~MonsterManager()
{
    CC_SAFE_DELETE(_pMonsterVec);
}

bool MonsterManager::init()
{
    bool bRet = false;
    do
    {
        _pMonsterVec = new Vector<MonsterBase *>;
        registerDeadEvent();
		registerReactionEvent();
        bRet = true;
    }
    while (0);
    
    return bRet;
}

Vector<MonsterBase*> &MonsterManager::getMonsterVec()
{
    return *_pMonsterVec;
}

void MonsterManager::addMonster(MonsterBase *pMonster)
{
    if(_pMonsterVec) _pMonsterVec->pushBack(pMonster);
    _funcAddMonsterToLayer(pMonster);
}

void MonsterManager::setFuncAddMonsterToLayer(const std::function<void (Entity*)> &rFuncAddMonsterToLayer)
{
    _funcAddMonsterToLayer = rFuncAddMonsterToLayer;
}

void MonsterManager::registerDeadEvent()
{
    NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::removeMonsterFromManager), "MonsterDead", nullptr);
    NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::openCheckGameEnd), "openCheckGameEnd", nullptr);
}


void MonsterManager::removeMonsterFromManager(Ref *pMonster)
{
    if(!(_pMonsterVec && _pMonsterVec->size())) return;
    _pMonsterVec->eraseObject(reinterpret_cast<MonsterBase*>(pMonster));
    
    if(_bIsOpenCheckGameEnd)
    {
        if(!_pMonsterVec->size())
        {
            auto tEnGameWin = en_GameWin;
            if(GameManager::getInstance()->getICarrotHp() >= 1) NOTIFY->postNotification("gameEnd", reinterpret_cast<Ref*>(&tEnGameWin));
            _bIsOpenCheckGameEnd = false;
        }
    }
}

void MonsterManager::clearManager()
{
    _pMonsterVec->clear();
}

void MonsterManager::openCheckGameEnd(cocos2d::Ref *pSender)
{
    _bIsOpenCheckGameEnd = true;
}

void MonsterManager::registerReactionEvent()
{
	NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::monsterReaction), "affectOthers", nullptr);
	NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::Electro_Charged), "Electro_Charged", nullptr);
	NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::OverLoad), "OverLoad", nullptr);
	NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::Superconduct), "Superconduct", nullptr);
	NOTIFY->addObserver(this, callfuncO_selector(MonsterManager::Shattered), "Shattered", nullptr);
}

void MonsterManager::OverLoad(Ref *pData) {
	AtkProperty atk;
	atk._enAtkState = 1;
	atk._iAtk = 10;
	atk._iDuration = 0;
	atk._elementType = Element::Pyro;

	auto tValue = *(reinterpret_cast<std::tuple<const VictimEntityBase*>*>(pData));
	auto victim = (MonsterBase*)std::get<0>(tValue);
	if (victim->getIsDead() || _pMonsterVec == nullptr)return;
	victim->beHurt2(atk);
	for (int i = 0; i < _pMonsterVec->size(); i++) {
		MonsterBase* iterMonster = (*_pMonsterVec).at(i);//降低血压，远离iterator

		if (iterMonster->getIHp() <= 0 || iterMonster->getIsDead())
		{
			continue;
		}
		const int overloadScale = 160;
		Vec2 p1 = iterMonster->getPosition();
		Vec2 p2 = victim->getPosition();
		if (victim != iterMonster && p1.distance(p2) < overloadScale) {
			iterMonster->showReaction("超载", Color3B::MAGENTA);
			iterMonster->beHurt2(atk);
		}

	}

}

void MonsterManager::Superconduct(Ref *pData) {

	AtkProperty atk;
	atk._enAtkState = 1;
	atk._iAtk = 10;
	atk._iDuration = 0;
	atk._elementType = Element::Cryo;

	auto tValue = *(reinterpret_cast<std::tuple<const VictimEntityBase*>*>(pData));
	auto victim = (MonsterBase*)std::get<0>(tValue);
	if (victim->getIsDead() || _pMonsterVec == nullptr)return;
	victim->beHurt2(atk);
	for (int i = 0; i < _pMonsterVec->size(); i++) {
		MonsterBase* iterMonster = (*_pMonsterVec).at(i);//降低血压，远离iterator

		if (iterMonster->getIHp() <= 0 || iterMonster->getIsDead())
		{
			continue;
		}
		const int overloadScale = 160;
		Vec2 p1 = iterMonster->getPosition();
		Vec2 p2 = victim->getPosition();
		if (victim != iterMonster && p1.distance(p2) < overloadScale) {
			iterMonster->showReaction("超导", Color3B(128, 0, 128));
			iterMonster->beHurt2(atk);
		}

	}

}

void MonsterManager::Electro_Charged(Ref *pData)
{

	AtkProperty eatk;
	eatk._enAtkState = 1;
	eatk._iAtk = 10;
	eatk._iDuration = 0;
	eatk._elementType = Element::Electro;

	auto tValue = *(reinterpret_cast<std::tuple<const VictimEntityBase*>*>(pData));
	auto victim = (MonsterBase*)std::get<0>(tValue);
	if (victim->getIsDead()|| _pMonsterVec == nullptr)return;
	
	for (int i = 0; i < _pMonsterVec->size(); i++) {
		MonsterBase* iterMonster = (*_pMonsterVec).at(i);//降低血压，远离iterator
	
		if (iterMonster->getIHp() <= 0 || iterMonster->getIsDead())
		{
			continue;
		}
		const int electroScale = 160;
		Vec2 p1 = iterMonster->getPosition();
		Vec2 p2 = victim->getPosition();
		if (victim != iterMonster && p1.distance(p2) < electroScale) {
			auto tValue = std::make_tuple(p1, p2);
			NOTIFY->postNotification("drawElectro", reinterpret_cast<Ref*>(&tValue));
			iterMonster->showReaction("感电", Color3B(128, 0, 128));
			iterMonster->beHurt2(eatk);
		}
		
	}
	
}

void MonsterManager::Shattered(Ref *pData)
{

	auto tValue = *(reinterpret_cast<std::tuple<Element, const VictimEntityBase*>*>(pData));
	auto e = std::get<0>(tValue);
	auto victim = std::get<1>(tValue);

	AtkProperty atk;
	atk._enAtkState = 1;
	atk._iAtk = 5;
	atk._iDuration = 0;
	atk._elementType = e;

	for (int i = 0; i < _pMonsterVec->size(); i++) {

		MonsterBase* iterMonster = (*_pMonsterVec).at(i);//降低血压，远离iterator

		if (iterMonster->getIHp() <= 0 || iterMonster->getIsDead())
		{
			continue;
		}
		const int ShatteredScale = 250;
		Vec2 p1 = iterMonster->getPosition();
		Vec2 p2 = victim->getPosition();
		if (victim != iterMonster && p1.distance(p2) < ShatteredScale) {
			iterMonster->showReaction("结晶", Color3B::YELLOW);
			iterMonster->setElement(Element::Clear);
			iterMonster->beHurt2(atk);
		}

	}
}

void MonsterManager::monsterReaction(Ref *pData)
{
	
	auto tValue = *(reinterpret_cast<std::tuple<Element, Element,const VictimEntityBase*>*>(pData));
	auto e1 = std::get<0>(tValue);
	auto e2 = std::get<1>(tValue);
	auto victim = std::get<2>(tValue);
	Element e;
	if (e1 == Element::Anemo) {
		e = e2;
	}
	else {
		e = e1;
	}

	AtkProperty atk;
	atk._enAtkState = 1;
	atk._iAtk = 10;
	atk._iDuration = 0;

	atk._elementType = e;

	for (int i = 0; i < _pMonsterVec->size(); i++) {

		MonsterBase* iterMonster = (*_pMonsterVec).at(i);//降低血压，远离iterator

		if (iterMonster->getIHp() <= 0 || iterMonster->getIsDead())
		{
			continue;
		}
		const int swirlScale = 160;
		Vec2 p1 = iterMonster->getPosition();
		Vec2 p2 = victim->getPosition();
		if (victim != iterMonster && p1.distance(p2) < swirlScale) {
			iterMonster->showReaction("扩散", Color3B(57, 255, 197));
			iterMonster->setElement(Element::Clear);
			iterMonster->beHurt2(atk);
		}

	}

}





