#include "GameTargetNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"

USING_NS_CC;

GameTargetNode* GameTargetNode::createLayout(char* pMainWord, const int& iCurrentLevel)
{
	GameTargetNode* pGameTargetNode = new GameTargetNode();
	if(pGameTargetNode->initLayout(pMainWord))
	{
		pGameTargetNode->m_iCurrentLevel = iCurrentLevel;
		pGameTargetNode->autorelease();
		return pGameTargetNode;
	}

	CC_SAFE_DELETE(pGameTargetNode);
	return NULL;
}

bool GameTargetNode::initLayout(char* pMainWord)
{
	if (!Node::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(191, 103, 241, 229));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { this->onTouchBackground(touch, event); return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	m_pBackgroundBoard = Sprite::create("Target-End-Game/Target_Board.png");
	m_pBackgroundBoard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 80));
	this->addChild(m_pBackgroundBoard);

	Label* pLabelMainWord = Label::createWithTTF(pMainWord, "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setPosition(Point(225, 580));
	this->addChild(pLabelMainWord);

	MenuItemImage* pPlayLevel = MenuItemImage::create(
		"Target-End-Game/Play_Button.png",
		"Target-End-Game/Play_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuPlayLevelCallBack, this));

	Menu* pMenuPlay = Menu::create(pPlayLevel, NULL);
	pMenuPlay->setPosition(Point(this->getContentSize().width/2.0f, 385));
	this->addChild(pMenuPlay);

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/Close_Button.png",
		"Target-End-Game/Close_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuCloseCallBack, this));

	Menu* pMenuClose = Menu::create(pCloseItem, NULL);
	pMenuClose->setPosition(Point(561, 790));
	this->addChild(pMenuClose, 10);

	MenuItemImage* pDictItem = MenuItemImage::create(
		"Target-End-Game/Dict_Button.png",
		"Target-End-Game/Dict_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuOpenDictCallBack, this));

	Menu* pMenuDict = Menu::create(pDictItem, NULL);
	pMenuDict->setPosition(Point(150, 380));
	this->addChild(pMenuDict, 10);

	return true;
}		

void GameTargetNode::menuPlayLevelCallBack()
{
	CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel);
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void GameTargetNode::menuCloseCallBack()
{
	this->getParent()->removeChild(this);
}

void GameTargetNode::menuOpenDictCallBack()
{
	DictionaryNode* pDictionary = DictionaryNode::create();
	this->addChild(pDictionary, 10);
}

void GameTargetNode::onTouchBackground(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	Point location = pTouch->getLocationInView();
	location = Director::sharedDirector()->convertToGL(location);

	if (location.x >= m_pBackgroundBoard->getPositionX() - m_pBackgroundBoard->getContentSize().width/2.0f + 20 && 
		location.x <= m_pBackgroundBoard->getPositionX() + m_pBackgroundBoard->getContentSize().width/2.0f - 20 &&
		location.y >= m_pBackgroundBoard->getPositionY() - m_pBackgroundBoard->getContentSize().height/2.0f + 20 &&
		location.y <= m_pBackgroundBoard->getPositionY() + m_pBackgroundBoard->getContentSize().height/2.0f - 10)
	{
		pEvent->stopPropagation();
		 return;
	}

	this->getParent()->removeChild(this);
	pEvent->stopPropagation();
}