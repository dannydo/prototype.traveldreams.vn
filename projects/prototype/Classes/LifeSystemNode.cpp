#include "LifeSystemNode.h"
#include "ButtonManagerNode.h"
#include "GetMoreLifeNode.h"
#include "FunctionCommon.h"

USING_NS_CC;

LifeSystemNode::LifeSystemNode()
{

}

LifeSystemNode::~LifeSystemNode()
{
	
}

bool LifeSystemNode::init()
{
	if (!Node::init())
	{
		return false;
	}	

	UserTable::getInstance()->updateLife(0);
	m_userInfo = UserTable::getInstance()->getUserInfo();

	m_pBackgroundClock = Sprite::create("Footer/btn_lives_section.png");

	ButtonNode* pButtonGetLife = ButtonNode::createButtonSprite(m_pBackgroundClock, CC_CALLBACK_1(LifeSystemNode::clickGetLife, this));
	pButtonGetLife->setAnchorPoint(Point(0.0f, 0.5f));
	pButtonGetLife->setPosition(Point(95.0f, 0.0f));

	ButtonManagerNode* pButtonManageNode = ButtonManagerNode::create();
	pButtonManageNode->addButtonNode(pButtonGetLife);
	this->addChild(pButtonManageNode);

	String clock = formatSecondsToDiaplay(m_userInfo.iLifeTimeRemaining);
	m_pLabelSecondsRemaing = LabelBMFont::create(clock.getCString(), "fonts/font_small_alert.fnt");
	m_pLabelSecondsRemaing->setAnchorPoint(Point(0.5f, 0.5f));
	m_pLabelSecondsRemaing->setPosition(Point(120.0f, 29.0f));
	m_pBackgroundClock->addChild(m_pLabelSecondsRemaing);

	char sLife[2];
	sprintf(sLife, "%d", m_userInfo.iLife);
	m_pLabelLife = LabelBMFont::create(sLife, "fonts/font_small_alert.fnt");
	m_pLabelLife->setAnchorPoint(Point(0.0f, 0.5f));
	m_pLabelLife->setPosition(Point(30.0f, 28.0f));
	m_pBackgroundClock->addChild(m_pLabelLife);

	if (m_userInfo.iLife == _MAX_LIFE_)
	{
		m_pLabelSecondsRemaing->setString("Full");
	}

	this->schedule(schedule_selector(LifeSystemNode::updateWhenTimeChange), 1.0f);
	return true;
}

void LifeSystemNode::clickGetLife(cocos2d::Object* sender)
{
	GetMoreLifeNode* pGetMoreLife = GetMoreLifeNode::create();
	pGetMoreLife->setGetMoreLifeType(GetMoreLifeType::eClosePopup);
	this->getParent()->addChild(pGetMoreLife);
}

void LifeSystemNode::updateWhenTimeChange(float dt)
{
	m_userInfo.iLifeTimeRemaining--;
	if(m_userInfo.iLifeTimeRemaining == 0)
	{
		m_userInfo.iLife++;
		if (m_userInfo.iLife < _MAX_LIFE_)
		{
			m_userInfo.iLifeTimeRemaining = _SECONDS_FOR_NEW_LIFE_;
		}
		else
		{
			m_userInfo.iLife = _MAX_LIFE_;
			this->unschedule(schedule_selector(LifeSystemNode::updateWhenTimeChange));
		}

		m_userInfo.ulLifeTimeBeginRemain = getTimeLocalCurrent();
		UserTable::getInstance()->updateUser(m_userInfo);
	}

	String clock = formatSecondsToDiaplay(m_userInfo.iLifeTimeRemaining);
	char sLife[2];
	sprintf(sLife, "%d", m_userInfo.iLife);
	m_pLabelSecondsRemaing->setString(clock.getCString());
	m_pLabelLife->setString(sLife);

	if (m_userInfo.iLife == _MAX_LIFE_)
	{
		m_pLabelSecondsRemaing->setString("Full");
	}
}

String LifeSystemNode::formatSecondsToDiaplay(const int& iSeconds)
{
	String str = "";
	if(iSeconds/60 < 10)
		str.append("0");
	str.appendWithFormat("%d", iSeconds/60);

	str.appendWithFormat("%s", ":");

	if(iSeconds%60 < 10)
		str.append("0");	
	str.appendWithFormat("%d", iSeconds%60);

	return str;
}