#include "GetMoreLifeNode.h"
#include "GameConfigManager.h"
#include "EndGameNode.h"
#include "MainMenuScene.h"

USING_NS_CC;

bool GetMoreLifeNode::init()
{
	if (!Node::init())
	{
		return false;
	}

	UserTable::getInstance()->updateLife(0);
	m_userInfo = UserTable::getInstance()->getUserInfo();
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("GetMoreLife/panel_noheader.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pFrameClock = Sprite::create("GetMoreLife/frame_out_moves.png");
	pFrameClock->setPosition(Point(320.0f, 657.0f));
	this->addChild(pFrameClock);

	Sprite* pHeartIcon = Sprite::create("GetMoreLife/hearth.png");
	pHeartIcon->setPosition(Point(200.0f, 660.0f));
	this->addChild(pHeartIcon);

	LabelBMFont *pLabelTitle = LabelBMFont::create("GET MORE LIVES", "fonts/font_title-popup.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelTitle->setPosition(Point(320.0f, 850.0f));
	this->addChild(pLabelTitle);

	LabelBMFont *pLabelDesc = LabelBMFont::create("Time to next life", "fonts/font-small-green-alert.fnt");
	pLabelDesc->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelDesc->setPosition(Point(320.0f, 745.0f));
	this->addChild(pLabelDesc);

	Sprite* pButtonBuySprite = Sprite::create("GetMoreLife/btn_playon.png");
	ButtonNode* buttonBuyNode = ButtonNode::createButtonSprite(pButtonBuySprite, CC_CALLBACK_1(GetMoreLifeNode::clickBuy, this));
	buttonBuyNode->setPosition(Point(320.0f, 500.0f));
	
	Sprite* pButtonAskFriendSprite = Sprite::create("GetMoreLife/btn_ask.png");
	ButtonNode* buttonAskFriendNode = ButtonNode::createButtonSprite(pButtonAskFriendSprite, CC_CALLBACK_1(GetMoreLifeNode::clickAskFriend, this));
	buttonAskFriendNode->setPosition(Point(320.0f, 385.0f));

	Sprite* pButtonCloseSprite = Sprite::create("GetMoreLife/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(GetMoreLifeNode::clickClose, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonBuyNode);
	pButtonManagerNode->addButtonNode(buttonAskFriendNode);
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(pButtonManagerNode);

	String clock = formatSecondsToDiaplay(m_userInfo.iLifeTimeRemaining);
	m_pLabelSecondsRemaing = LabelBMFont::create(clock.getCString(), "fonts/font_title-popup.fnt");
	m_pLabelSecondsRemaing->setPosition(Point(372.0f, 657.0f));
	this->addChild(m_pLabelSecondsRemaing);

	char sLife[2];
	sprintf(sLife, "%d", m_userInfo.iLife);
	m_pLabelLife = LabelBMFont::create(sLife, "fonts/font_title-popup.fnt");
	m_pLabelLife->setPosition(Point(217.0f, 657.0f));
	m_pLabelLife->setScale(1.4f);
	this->addChild(m_pLabelLife);

	if (m_userInfo.iLife == _MAX_LIFE_)
	{
		m_pLabelSecondsRemaing->setString("Full");
	}

	this->schedule(schedule_selector(GetMoreLifeNode::updateWhenTimeChange), 1.0f);

	return true;
}

void GetMoreLifeNode::updateWhenTimeChange(float dt)
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
			this->unschedule(schedule_selector(GetMoreLifeNode::updateWhenTimeChange));
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

void GetMoreLifeNode::setGetMoreLifeType(GetMoreLifeType getMoreLifeType)
{
	m_GetMoreLifeType = getMoreLifeType;
}


void GetMoreLifeNode::clickBuy(Object* sender)
{
	
}

void GetMoreLifeNode::clickAskFriend(Object* sender)
{
	
}

void GetMoreLifeNode::clickClose(Object* sender)
{
	switch (m_GetMoreLifeType)
	{
		case GetMoreLifeType::eClosePopup:
		{
			this->getParent()->removeChild(this);

			break;
		}
		case GetMoreLifeType::eGoToMainMenu:
		{
			Breadcrumb::getInstance()->resetSceneNodeToMainMenu();
			MainMenuScene* pMainMenu = MainMenuScene::create();
			Director::getInstance()->replaceScene(pMainMenu);

			break;
		}
		default:
			break;
	}
}

String GetMoreLifeNode::formatSecondsToDiaplay(const int& iSeconds)
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

unsigned long GetMoreLifeNode::getTimeLocalCurrent()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec + now.tv_usec/1000000 ; //seconds
	return iCurrentTime;
}