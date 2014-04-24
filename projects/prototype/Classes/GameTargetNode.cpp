#include "GameTargetNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "LeaderBoardNode.h"
#include "Database\LevelTable.h"
#include "ButtonManagerNode.h"
#include "ButtonNode.h"
#include "FunctionCommon.h"

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
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	//m_pSpriteBatchNode = SpriteBatchNode::create("Target-End-Game/TargetEndgame.pvr.ccz");
	//SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Target-End-Game/TargetEndgame.plist");
	//this->addChild(m_pSpriteBatchNode);

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	this->generateLayoutStartAndBonusQuest();

	Sprite* pIconBoosterImage = Sprite::create("Target-End-Game/icon-boosters.png");
	pIconBoosterImage->setPosition(Point(320.0f, 437.0f));
	this->addChild(pIconBoosterImage);

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(pMainWord);;
	sPath.append("/FlashCard/");
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(240.0f, 600.0f));
	this->addChild(pFlashCardImage);

	char sLevel[20];
	int iCalLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId);
	sprintf(sLevel, "Level %d", m_iCurrentLevel + iCalLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/font-bechic.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelLevel);

	Sprite* pButtonPlayGameSprite = Sprite::create("Target-End-Game/btn_big_play.png");
	ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonPlayGameSprite, CC_CALLBACK_1(GameTargetNode::menuPlayLevelCallBack, this));
	buttonPlayNode->setScale(0.9f);
	buttonPlayNode->setPosition(Point(320.0f, 293.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(GameTargetNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonPlayNode);
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(pButtonManagerNode);

	LeaderBoardtNode* pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel, m_sChapterId);
	pLeaderBoard->setPosition(Point(320.0f, 109.0f));
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
	int iTotalBonusQuest = 2;//pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount;

	LevelInfo levelInfo = LevelTable::getInstance()->getLevel(m_sChapterId, m_iCurrentLevel);

	for(int iIndex=0; iIndex<3; iIndex++) 
	{
		Sprite* pStarImage;
		if (levelInfo.iStar > iIndex)
		{
			pStarImage = Sprite::create("Target-End-Game/star_win_small.png");
			if (iIndex == 1)
				pStarImage->setScale(1.4f);
		}
		else
		{
			pStarImage = Sprite::create("Target-End-Game/star_target_small.png");
			if (iIndex == 1)
				pStarImage->setScale(1.4f);
		}
		pStarImage->setPosition(Point(220.0f + iIndex*100.0f - iTotalBonusQuest*46, 760.0f));
		this->addChild(pStarImage);
	}

	for(int iIndex=0; iIndex<iTotalBonusQuest; iIndex++) 
	{
		Sprite* pMushRoom;
		if (levelInfo.iBonusQuest > iIndex)
		{
			pMushRoom = Sprite::create("Target-End-Game/mushroom_win.png");
		}
		else
		{
			pMushRoom = Sprite::create("Target-End-Game/mushroom_fail.png");
		}
		pMushRoom->setPosition(Point(526.0f + iIndex*91.0f - iTotalBonusQuest*50, 760.0f));
		this->addChild(pMushRoom);
	}

	LabelBMFont *pLabelScore = LabelBMFont::create("SCORE:", "fonts/font_score.fnt");
	pLabelScore->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelScore->setPosition(Point(474.0f, 630.0f));
	this->addChild(pLabelScore);

	String sScore = "---";
	if (levelInfo.iScore > 0)
		sScore = formatNumber(levelInfo.iScore).getCString();

	LabelBMFont *pLabelNumScore = LabelBMFont::create(sScore.getCString(), "fonts/font_score.fnt");
	pLabelNumScore->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelNumScore->setPosition(Point(474.0f, 585.0f));
	pLabelNumScore->setScale(1.4f);
	this->addChild(pLabelNumScore);
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