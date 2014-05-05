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
	pLifeNode->setPosition(Point(30.0f, 930.0f));
	this->addChild(pLifeNode);

	Sprite* pGoldSprite = Sprite::create("Footer/btn_gold_section.png");
	ButtonNode* pButtonGold = ButtonNode::createButtonSprite(pGoldSprite, CC_CALLBACK_1(HeaderNode::clickBuyGold, this));
	pButtonGold->setPosition(Point(370.0f, 930.0f));

	char sGold[10];
	sprintf(sGold, "%d", userInfo.iMonney);
	m_pLabelGold = LabelBMFont::create(sGold, "fonts/font_small_alert.fnt");
	m_pLabelGold->setPosition(Point(130.0f, 29.0f));
	pGoldSprite->addChild(m_pLabelGold);

	Sprite* pMailSprite = Sprite::create("Footer/icon_mail.png");
	ButtonNode* pButtonMail = ButtonNode::createButtonSprite(pMailSprite, CC_CALLBACK_1(HeaderNode::clickMail, this));
	pButtonMail->setPosition(Point(520.0f, 930.0f));

	ButtonManagerNode* pButtonManageNode = ButtonManagerNode::create();
	pButtonManageNode->addButtonNode(pButtonGold);
	pButtonManageNode->addButtonNode(pButtonMail);
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
	pWifiSprite->setPosition(Point(587.0f, 930.0f));
#endif

	return true;
}

void HeaderNode::clickBuyGold(cocos2d::Object* sender)
{

}

void HeaderNode::clickMail(cocos2d::Object* sender)
{

}

void HeaderNode::updateLayoutMonney(const int& iMonney)
{
	char sGold[10];
	sprintf(sGold, "%d", iMonney);
	m_pLabelGold->setString(sGold);
}