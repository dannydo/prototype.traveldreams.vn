#include "HeaderNode.h"
#include "LifeSystemNode.h"
#include "ButtonManagerNode.h"

USING_NS_CC;


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

HeaderNode::~HeaderNode()
{
	
}

bool HeaderNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();

	Sprite* pBarTop = Sprite::create("Footer/header_bar.png");
	pBarTop->setPosition(Point(320.0f, 918.0f));
	this->addChild(pBarTop);

	LifeSystemNode* pLifeNode = LifeSystemNode::create();
	pLifeNode->setPosition(Point(32.0f, 930.0f));
	this->addChild(pLifeNode);

	Sprite* pGoldSprite = Sprite::create("Footer/btn_gold_section.png");
	ButtonNode* pButtonGold = ButtonNode::createButtonSprite(pGoldSprite, CC_CALLBACK_1(HeaderNode::clickBuyGold, this));
	pButtonGold->setPosition(Point(415.0f, 930.0f));

	char sGold[10];
	sprintf(sGold, "%d", userInfo.iMonney);
	LabelBMFont* pLabelGold = LabelBMFont::create(sGold, "fonts/font_small_alert.fnt");
	pLabelGold->setPosition(Point(17.0f, -2.0f));
	pButtonGold->addChild(pLabelGold);

	ButtonManagerNode* pButtonManageNode = ButtonManagerNode::create();
	pButtonManageNode->addButtonNode(pButtonGold);
	this->addChild(pButtonManageNode);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
	if (FacebookManager::getInstance()->isNetworkOnline())
	{
		Sprite* pWifiSprite = Sprite::create("Footer/have_wifi.png");
		pWifiSprite->setPosition(Point(585.0f, 930.0f));
		this->addChild(pWifiSprite);
	}
	else
	{
		Sprite* pWifiSprite = Sprite::create("Footer/no_wifi.png");
		pWifiSprite->setPosition(Point(585.0f, 930.0f));
		this->addChild(pWifiSprite);
	}
#else
	Sprite* pWifiSprite = Sprite::create("Footer/have_wifi.png");
	this->addChild(pWifiSprite);
	pWifiSprite->setPosition(Point(585.0f, 930.0f));
#endif

	return true;
}

void HeaderNode::clickBuyGold(cocos2d::Object* sender)
{

}