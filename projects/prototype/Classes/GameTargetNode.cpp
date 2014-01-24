#include "GameTargetNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"
#include "LeaderBoardNode.h"

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
	m_pBackgroundBoard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 115));
	this->addChild(m_pBackgroundBoard);

	//Sprite* pTargetCard = Sprite::create("Target-End-Game/Target_Card.png");
	//pTargetCard->setPosition(Point(320, 537));
	//this->addChild(pTargetCard);

	Sprite* pBackgroundTargetCard = Sprite::create("Target-End-Game/Win_Card.png");
	pBackgroundTargetCard->setPosition(Point(320, 600));
	this->addChild(pBackgroundTargetCard);

	std::string sPath = "FlashCard/";
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCard = Sprite::create(sPath.c_str());
	pFlashCard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 96));
	this->addChild(pFlashCard, 10);

	Node* pLevelNode = this->generateLayoutLevel(m_iCurrentLevel);
	pLevelNode->setPosition(Point(365, 905));
	this->addChild(pLevelNode);

	Label* pLabelMainWord = Label::createWithTTF("???", "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelMainWord->setPosition(Point(315, 645));
	this->addChild(pLabelMainWord);

	MenuItemImage* pPlayLevel = MenuItemImage::create(
		"Target-End-Game/Play_Button.png",
		"Target-End-Game/Play_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuPlayLevelCallBack, this));
	pPlayLevel->setPosition(Point(this->getContentSize().width/2.0f, 370));

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/Close_Button.png",
		"Target-End-Game/Close_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuCloseCallBack, this));
	pCloseItem->setPosition(Point(561, 855));

	/*
	MenuItemImage* pDictItem = MenuItemImage::create(
		"Target-End-Game/Dict_Button.png",
		"Target-End-Game/Dict_Button_Click.png",
		CC_CALLBACK_0(GameTargetNode::menuOpenDictCallBack, this));
	pDictItem->setPosition(Point(150, 300));
	*/

	Menu* pMenu = Menu::create(pPlayLevel, pCloseItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu, 10);

	LeaderBoardtNode* pLeaderBoard = LeaderBoardtNode::create();
	pLeaderBoard->setPosition(Point(320, 136));
	this->addChild(pLeaderBoard);

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

Node* GameTargetNode::generateLayoutLevel(int iLevel)
{
	std::vector<int> arrNumber = this->generateArrayNumber(iLevel);
	int width = 0;
	int height;
	Node* pNode = Node::create();
	int iTotal = arrNumber.size();
	
	while(!arrNumber.empty())
	{
		char sFileName[50];
		sprintf(sFileName, "Target-End-Game/NumScore/NumScore_%d.png", arrNumber.back());
		Sprite* sprite = Sprite::create(sFileName);

		height = sprite->getContentSize().height;
		sprite->setPosition(Point(width, height/2.0f));
		width = width + sprite->getContentSize().width - 15;
		pNode->addChild(sprite);
		arrNumber.pop_back();
	}
	pNode->setContentSize(CCSizeMake(width - 18, height));
	pNode->setAnchorPoint(Point(0.0f, 0.5f));
	pNode->setScale(1.3f);

	return pNode;
}

std::vector<int> GameTargetNode::generateArrayNumber(int iNumber)
{
	int iTemp = iNumber;
	std::vector<int> arrNumber;
	while(iTemp >= 1)
	{
		arrNumber.push_back(iTemp%10);
		iTemp = iTemp/10.0f;
	}

	return arrNumber;
}