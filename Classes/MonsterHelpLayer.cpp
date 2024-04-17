//
//  MonsterHelpLayer.cpp
//  CarrotFantasy
//
//  Created by Yan on 14-10-6.
//
//

#include "MonsterHelpLayer.h"


bool MonsterHelpLayer::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!Layer::init());
        
        bRet = true;
    }
    while (0);
	/*auto monsterHelpLayer = Sprite::createWithSpriteFrameName("help_monster.png");	
	monsterHelpLayer->setPosition(480,280);
	monsterHelpLayer->setScale(0.95f);
	addChild(monsterHelpLayer);*/
	std::string content = "    癸卯年中，C女士见鸡窝日益兴起，忆往昔堕胎之事，怀恨在心，遂率众怪入侵鸡窝。坤家军见势连夜升级曾经守护过萝卜的炮塔，借提瓦特七神之力赋予炮塔元素，以抵御众怪之入侵，誓死捍卫鸡窝";
	label = Label::createWithTTF(content,"fonts/zh-cn.ttf",36);
	label->enableShadow();
	label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	label->setPosition(VisibleRect::center());
	label->setDimensions(640, 0);
	label->setOpacity(0);
	addChild(label);
	
	//不知道为毛一直报错
	//static int i = 1;
	//schedule([&](float dlt) {
	//	if (i > content.length())
	//	{
	//		unschedule("prtstr");
	//		return;
	//	}
	//	
	//	std::string str = content.substr(0,i);
	//	label->setString(str);
	//	i++;
	//	
	//},0.1f,"prtstr");
    return bRet;
}

void MonsterHelpLayer::onEnter()
{
	Layer::onEnter();
	CCLOG("Layer entered!");
	label->runAction(FadeIn::create(0.5f));
}
