#include "GameTargetNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "LeaderBoardNode.h"
#include "Database\LevelTable.h"
#include "ButtonManagerNode.h"
#include "ButtonNode.h"

USING_NS_CC;

GameTargetNode* GameTargetNode::createLayout(const Word& pMainWord, const int& iCurrentLevel, const std::string sChapterId)
{
	GameTargetNode* pGameTargetNode = new GameTargetNode();
	pGameTargetNode->m_iCurrentLevel = iCurrentLevel;
	pGameTargetNode->m_sChapterId = sChapterId;

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

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	m_pSpriteBatchNode = SpriteBatchNode::create("Target-End-Game/TargetEndgame.pvr.ccz");
	SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Target-End-Game/TargetEndgame.plist");
	this->addChild(m_pSpriteBatchNode);

	Sprite* pBackgroundBoard = Sprite::createWithSpriteFrameName("panel.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	m_pSpriteBatchNode->addChild(pBackgroundBoard);

	Sprite* pBackgroundFlashCard = Sprite::createWithSpriteFrameName("flashcard_board.png");
	pBackgroundFlashCard->setPosition(Point(320.0f, 695.0f));
	m_pSpriteBatchNode->addChild(pBackgroundFlashCard);

	this->generateLayoutStartAndBonusQuest();

	Sprite* pBoostersImage = Sprite::createWithSpriteFrameName("booster.png");
	pBoostersImage->setPosition(Point(216.0f, 531.0f));
	m_pSpriteBatchNode->addChild(pBoostersImage);

	Sprite* pIconBoosterImage = Sprite::createWithSpriteFrameName("icon-boosters.PNG");
	pIconBoosterImage->setPosition(Point(320.0f, 465.0f));
	m_pSpriteBatchNode->addChild(pIconBoosterImage);
	
	Sprite* pMonsterImage = Sprite::createWithSpriteFrameName("pet-r.png");
	pMonsterImage->setPosition(Point(548.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pMonsterImage);

	Sprite* pPetImage = Sprite::createWithSpriteFrameName("pet-l.png");
	pPetImage->setPosition(Point(102.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pPetImage);

	Sprite* pTargetImage = Sprite::createWithSpriteFrameName("target.png");
	pTargetImage->setPosition(Point(320.0f, 755.0f));
	pTargetImage->setRotation(-3.5);
	m_pSpriteBatchNode->addChild(pTargetImage);

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(pMainWord);;
	sPath.append("/FlashCard/");
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(320.0f, 665.0f));
	pFlashCardImage->setRotation(-3.5);
	this->addChild(pFlashCardImage);

	Sprite* pLevelImage = Sprite::createWithSpriteFrameName("level.png");
	pLevelImage->setPosition(Point(0.0f, 0.0f));

	char sLevel[10];
	int iCalLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId);
	sprintf(sLevel, "%d", m_iCurrentLevel + iCalLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/Level-bitmap-font-game.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(pLevelImage->getContentSize().width/2 + pLabelLevel->getContentSize().width/2.0f, 5.0f));
	
	Node* pNodeLevel = Node::create();
	pNodeLevel->addChild(pLabelLevel);
	pNodeLevel->addChild(pLevelImage);
	pNodeLevel->setPosition(Point(320.0f - pLevelImage->getContentSize().width/4.0f - pLabelLevel->getContentSize().width/4.0f + 22, 920.0f));
	this->addChild(pNodeLevel);

	Sprite* pButtonPlayGameSprite = Sprite::create("Target-End-Game/btn_play.png");
	ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonPlayGameSprite, CC_CALLBACK_1(GameTargetNode::menuPlayLevelCallBack, this));
	buttonPlayNode->setPosition(Point(320.0f, 335.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(GameTargetNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(582.0f, 914.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonPlayNode);
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(pButtonManagerNode);

	LeaderBoardtNode* pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel, m_sChapterId);
	pLeaderBoard->setPosition(Point(320, 114));
	this->addChild(pLeaderBoard);

	return true;
}		

void GameTargetNode::menuPlayLevelCallBack(Object* sender)
{	
	CCScene *pGameScene = HelloWorld::createScene();
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void GameTargetNode::menuCloseCallBack(Object* sender)
{
	this->getParent()->removeChild(this);
}

void GameTargetNode::generateLayoutStartAndBonusQuest()
{
	LevelConfig* pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig(m_sChapterId, m_iCurrentLevel);
	int iTotalBonusQuest = pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount;

	LevelInfo levelInfo = LevelTable::getInstance()->getLevel(m_sChapterId, m_iCurrentLevel);

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