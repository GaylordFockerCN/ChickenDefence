//
//  HelpMenuPanelLayer.cpp
//  CarrotFantasy
//
//  Created by Yan on 14-10-6.
//
//

#include "HelpMenuPanelLayer.h"
#include "HelpLayer.h"
#include "MonsterHelpLayer.h"
#include "TowerHelpLayer.h"
#include "SceneManager.h"

bool HelpMenuPanelLayer::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!Layer::init());
        
       /* _pLayers = LayerMultiplex::create(HelpLayer::create(), MonsterHelpLayer::create(), TowerHelpLayer::create(), nullptr);
        addChild(_pLayers);*/
     
        //auto pBg = Sprite::createWithSpriteFrameName("help_bg.png");
		auto pBg = Sprite::create("ikun/bg/bg00.png");
		addChild(pBg);
		pBg->setPosition(VisibleRect::center());
		auto doAction = Sprite::create("ikun/bg/bg00.png");
		//doAction->setPosition(VisibleRect::center());
		addChild(doAction);
		Animation * pAnimation = Animation::create();
		for (int i = 1; i <= 20; i++)
		{
			std::string SpriteFrameName = StringUtils::format("ikun/bg/bg0%d.png", i);
			SpriteFrame* sf = SpriteFrame::create(SpriteFrameName,Rect(0,0,960,640));
			pAnimation->addSpriteFrame(sf);
		}
		pAnimation->setDelayPerUnit(0.05);
		//doAction->setPosition(VisibleRect::center());
		doAction->runAction(RepeatForever::create(Animate::create(pAnimation)));

        createMenuPanel();
        
        bRet = true;
    }
    while (0);
    
    return bRet;
}

void HelpMenuPanelLayer::createMenuPanel()
{
    auto pHomeBtn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("help_home_normal.png"), Sprite::createWithSpriteFrameName("help_home_pressed.png"), [](Ref *pSender)
    {
        auto enWelcomeScene = std::make_tuple(en_WelcomeScene, 0);
        NOTIFY->postNotification("changeScene", reinterpret_cast<Ref*>(&enWelcomeScene));
    });
    pHomeBtn->setPosition(pHomeBtn->getContentSize().width / 2, 640 - pHomeBtn->getContentSize().height / 2 + 5);
    
	auto pNormalItem = MenuItemSprite::create(Sprite::createWithSpriteFrameName("tips_normal_CN.png"), nullptr);
	/*auto helpLayer = HelpLayer::create();
	helpLayer->setName("_helpLayer");
	addChild(helpLayer);*/
    auto pSelectedItem = MenuItemSprite::create(Sprite::createWithSpriteFrameName("tips_selected_CN.png"), nullptr);
    pHelpBtn = MenuItemToggle::createWithCallback([=](Ref *pSender){
		pMonsterHelpBtn->setSelectedIndex(0);
		pTowerHelpBtn->setSelectedIndex(0);
		this->removeChildByName("helpLayer");
		this->removeChildByName("monsterHelpLayer");
		this->removeChildByName("towerHelpLayer");
		if (pHelpBtn->getSelectedIndex() == 1)
		{
			auto helpLayer = HelpLayer::create();
			helpLayer->setName("helpLayer");
			addChild(helpLayer);
		}

	}, pNormalItem, pSelectedItem, NULL);
	//pHelpBtn->setSelectedIndex(1);
    pNormalItem = MenuItemSprite::create(Sprite::createWithSpriteFrameName("monster_normal_CN.png"), nullptr);
    pSelectedItem = MenuItemSprite::create(Sprite::createWithSpriteFrameName("monster_selected_CN.png"), nullptr);
    pMonsterHelpBtn = MenuItemToggle::createWithCallback([=](Ref *pSender){
		pHelpBtn->setSelectedIndex(0);
		pTowerHelpBtn->setSelectedIndex(0);

		this->removeChildByName("helpLayer");
		this->removeChildByName("monsterHelpLayer");
		this->removeChildByName("towerHelpLayer");
		
		if (pMonsterHelpBtn->getSelectedIndex() == 1)
		{
			auto monsterHelpLayer = MonsterHelpLayer::create();
			monsterHelpLayer->setName("monsterHelpLayer");
			addChild(monsterHelpLayer);
		}
	}, pNormalItem, pSelectedItem, NULL);
	auto monsterHelpLayer = MonsterHelpLayer::create();
	monsterHelpLayer->setName("monsterHelpLayer");
	addChild(monsterHelpLayer);
	pMonsterHelpBtn->setSelectedIndex(1);

    pNormalItem = MenuItemSprite::create(Sprite::createWithSpriteFrameName("tower_normal_CN.png"), nullptr);
    pSelectedItem = MenuItemSprite::create(Sprite::createWithSpriteFrameName("tower_selected_CN.png"), nullptr);
    pTowerHelpBtn = MenuItemToggle::createWithCallback([=](Ref *pSender){
		pHelpBtn->setSelectedIndex(0);
		pMonsterHelpBtn->setSelectedIndex(0);

		this->removeChildByName("helpLayer");
		this->removeChildByName("monsterHelpLayer");
		this->removeChildByName("towerHelpLayer");

		if (pTowerHelpBtn->getSelectedIndex() == 1)
		{
			auto towerHelpLayer = TowerHelpLayer::create();
			towerHelpLayer->setName("towerHelpLayer");
			addChild(towerHelpLayer);
		}
	}, pNormalItem, pSelectedItem, nullptr);

    
	
	
    auto pMenuPanel = Menu::create(pHomeBtn, pHelpBtn, pMonsterHelpBtn, pTowerHelpBtn, NULL);
	pHelpBtn->setPosition(320,590);
	pMonsterHelpBtn->setPosition(480, 590);
	pTowerHelpBtn->setPosition(640, 590);
	pMenuPanel->setPosition(0, 0);
    addChild(pMenuPanel,2);
}


