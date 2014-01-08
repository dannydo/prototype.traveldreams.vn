#include "GameTargetNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"

USING_NS_CC;

GameTargetNode* GameTargetNode::createLayout(const Word& pMainWord, const int& iCurrentLevel)
{
	GameTargetNode* pGameTargetNode = new GameTargetNode();
	pGameTargetNode->m_iCurrentLevel = iCurrentLevel;
	if(pGameTargetNode->initLayout(pMainWord))
	{
		pGameTargetNode->autorelease();
		return pGameTargetNode;
	}

	CC_SAFE_DELETE(pGameTargetNode);
	return NULL;
}

bool GameTargetNode::initLayout(const Word& pMainWord)
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
	m_pBackgroundBoard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f));
	this->addChild(m_pBackgroundBoard);

	//Sprite* pTargetCard = Sprite::create("Target-End-Game/Target_Card.png");
	//pTargetCard->setPosition(Point(320, 537));
	//this->addChild(pTargetCard);

	Sprite* pBackgroundTargetCard = Sprite::create("Target-End-Game/Win_Card.png");
	pBackgroundTargetCard->setPosition(Point(320, 535));
	this->addChild(pBackgroundTargetCard);

	std::string sPath = "FlashCard/";
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCard = Sprite::create(sPath.c_str());
	pFlashCard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 31));
	this->addChild(pFlashCard, 10);

	char sLevel[5];
	Label* pLabelLevel = Label::createWithTTF(itoa(m_iCurrentLevel, sLevel, 10), "fonts/ARLRDBD.ttf", 52);
	pLabelLevel->setColor(ccc3(219, 160, 74));
	pLabelLevel->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelLevel->setPosition(Point(350, 745));
	pLabelLevel->setRotationX(15);
	this->addChild(pLabelLevel);

	Label* pLabelMainWord = Label::createWithTTF("???", "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelMainWord->setPosition(Point(315, 580));
	this->addChild(pLabelMainWord);

	MenuItemImage* pPlayLevel = MenuItemImage::create(
		"Target-End-Game/Play_Button.png",
		"Target-End-Game/Play_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuPlayLevelCallBack, this));
	pPlayLevel->setPosition(Point(this->getContentSize().width/2.0f, 305));

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/Close_Button.png",
		"Target-End-Game/Close_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuCloseCallBack, this));
	pCloseItem->setPosition(Point(561, 790));

	MenuItemImage* pDictItem = MenuItemImage::create(
		"Target-End-Game/Dict_Button.png",
		"Target-End-Game/Dict_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuOpenDictCallBack, this));
	pDictItem->setPosition(Point(150, 300));

	Menu* pMenu = Menu::create(pPlayLevel, pCloseItem, pDictItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu, 10);

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