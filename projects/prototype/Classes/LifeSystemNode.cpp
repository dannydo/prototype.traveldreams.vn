#include "LifeSystemNode.h"

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
	UserTable::getInstance()->updateUser(m_userInfo);

	Sprite* pBackgroundClock = Sprite::create("Footer/life-time.png");
	pBackgroundClock->setAnchorPoint(Point(0.0f, 0.5f));
	pBackgroundClock->setPosition(Point(45.0f, 0.0f));
	this->addChild(pBackgroundClock);

	Sprite* pAddLifeButton = Sprite::create("Footer/add-life-btn.png");
	pAddLifeButton->setAnchorPoint(Point(0.0f, 0.5f));
	pAddLifeButton->setPosition(Point(116.5f, 25.5f));
	pBackgroundClock->addChild(pAddLifeButton);

	String clock = formatSecondsToDiaplay(m_userInfo.iLifeTimeRemaining);
	m_pLabelSecondsRemaing = CCLabelTTF::create(clock.getCString(), "fonts/UTM Cookies.ttf", 24);
	m_pLabelSecondsRemaing->setAnchorPoint(Point(0.0f, 0.5f));
	m_pLabelSecondsRemaing->setPosition(Point(42.0f, 29.0f));
	m_pLabelSecondsRemaing->setColor(ccc3(180, 115, 5));

	if (m_userInfo.iLifeTimeRemaining == 0)
	{
		m_pLabelSecondsRemaing->setVisible(false);
	}

	pBackgroundClock->addChild(m_pLabelSecondsRemaing);

	Sprite* pBackgroundLife = Sprite::create("Footer/hearth.png");
	pBackgroundLife->setAnchorPoint(Point(0.0f, 0.5f));
	pBackgroundLife->setPosition(Point(0.0f, 0.0f));
	this->addChild(pBackgroundLife);

	char sLife[2];
	sprintf(sLife, "%d", m_userInfo.iLife);
	m_pLabelLife = LabelBMFont::create(sLife, "fonts/Flashcard-bitmap-font-game.fnt");
	m_pLabelLife->setAnchorPoint(Point(0.0f, 0.5f));
	m_pLabelLife->setPosition(Point(32.0f, 5.0f));
	m_pLabelLife->setScale(1.2f);
	this->addChild(m_pLabelLife);

	this->schedule(schedule_selector(LifeSystemNode::updateWhenTimeChange), 1.0f);
	return true;
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
			m_pLabelSecondsRemaing->setVisible(false);
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

unsigned long LifeSystemNode::getTimeLocalCurrent()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec + now.tv_usec/1000000 ; //seconds
	return iCurrentTime;
}