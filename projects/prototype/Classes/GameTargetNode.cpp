#include "GameTargetNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"
#include "LeaderBoardNode.h"
#include "Database\LevelTable.h"

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

	LayerColor* pBackground = LayerColor::create(ccc4(101, 85, 130, 255));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { this->onTouchBackground(touch, event); return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	m_pSpriteBatchNode = SpriteBatchNode::create("Target-End-Game/TargetEndgame.pvr.ccz");
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Target-End-Game/TargetEndgame.plist");
	this->addChild(m_pSpriteBatchNode);

	Sprite* pBackgroundBoard = Sprite::createWithSpriteFrameName("panel.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	m_pSpriteBatchNode->addChild(pBackgroundBoard);

	Sprite* pLevelImage = Sprite::createWithSpriteFrameName("level.png");
	m_pSpriteBatchNode->addChild(pLevelImage);

	Sprite* pBackgroundFlashCard = Sprite::createWithSpriteFrameName("flashcard_board.png");
	pBackgroundFlashCard->setPosition(Point(320.0f, 695.0f));
	m_pSpriteBatchNode->addChild(pBackgroundFlashCard);

	this->generateLayoutStartAndBonusQuest();

	Sprite* pBoostersImage = Sprite::createWithSpriteFrameName("booster.png");
	pBoostersImage->setPosition(Point(216.0f, 531.0f));
	m_pSpriteBatchNode->addChild(pBoostersImage);
	
	Sprite* pMonsterImage = Sprite::createWithSpriteFrameName("monster.png");
	pMonsterImage->setPosition(Point(548.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pMonsterImage);

	Sprite* pPetImage = Sprite::createWithSpriteFrameName("pet.png");
	pPetImage->setPosition(Point(102.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pPetImage);

	std::string sPath = "FlashCard/flashcardimage/";
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(320.0f, 680.0f));
	pFlashCardImage->setRotation(-3.5);
	this->addChild(pFlashCardImage);		 

	Sprite* pTargetImage = Sprite::create("Target-End-Game/target.png");
	pTargetImage->setPosition(Point(320.0f, 755.0f));
	pTargetImage->setRotation(-3.5);
	this->addChild(pTargetImage);

	char sLevel[10];
	sprintf(sLevel, "%d", m_iCurrentLevel);
	LabelTTF* pLabelLevel = LabelTTF::create(sLevel, "Arial", 32);
	this->addChild(pLabelLevel);

	int iWidth = pLabelLevel->getContentSize().width+pLevelImage->getContentSize().width;
	pLabelLevel->setPosition(Point(320.0f+iWidth/4.0f+iWidth/4.0f, 920.0f));
	pLevelImage->setPosition(Point(320.0f+iWidth/4.0f-iWidth/4.0f, 920.0f));

	MenuItemImage* pPlayLevel = MenuItemImage::create(
		"Target-End-Game/btn_play.png",
		"Target-End-Game/btn_play.png",
		CC_CALLBACK_0(GameTargetNode::menuPlayLevelCallBack, this));
	pPlayLevel->setPosition(Point(320.0f, 335.0f));

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/btn_close.png",
		"Target-End-Game/btn_close.png",
		CC_CALLBACK_0(GameTargetNode::menuCloseCallBack, this));
	pCloseItem->setPosition(Point(582.0f, 914.0f));

	Menu* pMenu = Menu::create(pPlayLevel, pCloseItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu, 10);

	LeaderBoardtNode* pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel);
	pLeaderBoard->setPosition(Point(320, 114));
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

void GameTargetNode::onTouchBackground(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	pEvent->stopPropagation();
}

void GameTargetNode::generateLayoutStartAndBonusQuest()
{
	LevelConfig* pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig(m_iCurrentLevel);
	int iTotalBonusQuest = pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount;
	iTotalBonusQuest = 2;

	LevelInfo levelInfo = LevelTable::getInstance()->fetchLevel(m_iCurrentLevel);

	for(int iIndex=0; iIndex<3; iIndex++) 
	{
		Sprite* pStarImage;
		if (levelInfo.iStar > iIndex)
		{
			pStarImage = Sprite::createWithSpriteFrameName("star_yellow.png");
		}
		else
		{
			pStarImage = Sprite::createWithSpriteFrameName("star_purple.png");
		}
		pStarImage->setPosition(Point(200.0f + iIndex*108.0f - iTotalBonusQuest*46, 823.0f + iIndex*5 - iTotalBonusQuest*5));
		m_pSpriteBatchNode->addChild(pStarImage);
	}

	for(int iIndex=0; iIndex<iTotalBonusQuest; iIndex++) 
	{
		Sprite* pMushRoom;
		if (levelInfo.iBonusQuest > iIndex)
		{
			pMushRoom = Sprite::createWithSpriteFrameName("mushroom.png");
		}
		else
		{
			pMushRoom = Sprite::createWithSpriteFrameName("mushroom-fail.PNG");
		}
		pMushRoom->setPosition(Point(529.0f + iIndex*98.0f - iTotalBonusQuest*50, 835.0f + iIndex*5 + 3));
		m_pSpriteBatchNode->addChild(pMushRoom);
	}
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