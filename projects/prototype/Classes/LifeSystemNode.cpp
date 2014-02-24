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

	LabelTTF* labelLife = LabelTTF::create("Life: ", "Arial", 32);
	labelLife->setAnchorPoint(Point(0.0f, 0.5f));
	this->addChild(labelLife);

	String life = "";
	life.appendWithFormat("%d", m_userInfo.iLife);
	m_pLabelLife = LabelTTF::create(life.getCString(), "Arial", 32);
	m_pLabelLife->setAnchorPoint(Point(0.0f, 0.5f));
	m_pLabelLife->setPosition(Point(70.0f, 0.0f));
	this->addChild(m_pLabelLife);

	String clock = formatSecondsToDiaplay(m_userInfo.iLifeTimeRemaining);
	m_pLabelSecondsRemaing = LabelTTF::create(clock.getCString(), "Arial", 32);
	m_pLabelSecondsRemaing->setAnchorPoint(Point(0.0f, 0.5f));
	m_pLabelSecondsRemaing->setPosition(Point(110.0f, 0.0f));

	if (m_userInfo.iLifeTimeRemaining == 0)
	{
		m_pLabelSecondsRemaing->setVisible(false);
	}

	this->addChild(m_pLabelSecondsRemaing);

	
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
	String life = "";
	life.appendWithFormat("%d", m_userInfo.iLife);
	m_pLabelSecondsRemaing->setString(clock.getCString());
	m_pLabelLife->setString(life.getCString());
}

String LifeSystemNode::formatSecondsToDiaplay(const int& iSeconds)
{
	String str = "";
	str.appendWithFormat("%d", iSeconds/60);
	str.appendWithFormat("%s", ":");
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