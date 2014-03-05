#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"
#include "Database\UserTable.h"

USING_NS_CC;

EndGameNode* EndGameNode::createLayoutLose(const int& iScore, const Word& pMainWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();	  
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	if(pEndGameNode->initLose(iScore, pMainWord))
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

EndGameNode* EndGameNode::createLayoutWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();	  
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	pEndGameNode->m_iTotalBonusQuest = 0;
	if(pEndGameNode->initWin(iScore, pMainWord, pSubWord))
	{			
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool EndGameNode::initLose(const int& iScore, const Word& pMainWord)
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pBackgroundFail = Sprite::create("Target-End-Game/Fail_Board.png");
	pBackgroundFail->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 115));
	this->addChild(pBackgroundFail);

	Node* pLevelNode = this->generateLayoutLevel(m_iCurrentLevel);
	pLevelNode->setPosition(Point(365, 905));
	this->addChild(pLevelNode);

	Sprite* pBackgroundTargetCard = Sprite::create("Target-End-Game/Fail_Card.png");
	pBackgroundTargetCard->setPosition(Point(320, 600));
	this->addChild(pBackgroundTargetCard);

	Label* pLabelMainWord = Label::createWithTTF(pMainWord.m_sWord, "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelMainWord->setPosition(Point(315, 645));
	this->addChild(pLabelMainWord);

	MenuItemImage* pRetryLevel = MenuItemImage::create(
		"Target-End-Game/Retry_Button.png",
		"Target-End-Game/Retry_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuRetryLevelLoseCallBack, this));

	Menu* pMenuLose = Menu::create(pRetryLevel, NULL);
	pMenuLose->setPosition(Point(this->getContentSize().width/2.0f, 370));
	this->addChild(pMenuLose);

	return true;
}

bool EndGameNode::initWin(const int& iScore, const Word& pMainWord, const std::vector<Word>& pSubWord)
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pBackgroundShine = Sprite::create("Target-End-Game/Shine.png");
	pBackgroundShine->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height - 160));
	this->addChild(pBackgroundShine);

	Sprite* pBackgroundWin = Sprite::create("Target-End-Game/Win_Board.png");
	pBackgroundWin->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 115));
	this->addChild(pBackgroundWin);

	Node* pLevelNode = this->generateLayoutLevel(m_iCurrentLevel);
	pLevelNode->setPosition(Point(365, 905));
	this->addChild(pLevelNode);

	m_pStarNode = this->generateLayoutStar();
	m_pStarNode->setPosition(Point(228, 800));
	this->addChild(m_pStarNode);

	m_pBonusQuestNode = this->generateLayoutBonusQuest();
	m_pBonusQuestNode->setPosition(Point(500, 905));
	this->addChild(m_pBonusQuestNode);

	Node* pScoreNode = this->generateLayoutScore(iScore);
	pScoreNode->setPosition(Point(320, 745));

	this->addChild(pScoreNode);

	Sprite* pBackgroundTargetCard = Sprite::create("Target-End-Game/Win_Card.png");
	pBackgroundTargetCard->setPosition(Point(320, 600));
	this->addChild(pBackgroundTargetCard);

	std::string sPath = "FlashCard/flashcardimage/";
	sPath.append(pMainWord.m_sFlashCardImage);
	Sprite* pFlashCard = Sprite::create(sPath.c_str());
	pFlashCard->setPosition(Point(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f + 96));
	this->addChild(pFlashCard, 10);

	Label* pLabelMainWord = Label::createWithTTF(pMainWord.m_sWord, "fonts/ARLRDBD.ttf", 32);
	pLabelMainWord->setColor(ccc3(0, 0, 0));
	pLabelMainWord->setAnchorPoint(Point(0.5f, 0.0f));
	pLabelMainWord->setPosition(Point(315, 645));
	this->addChild(pLabelMainWord);

	Node* pSubWordNode = this->generateLayoutSubWord(pSubWord);
	pSubWordNode->setPosition(Point(320, 440));
	this->addChild(pSubWordNode);

	MenuItemImage* pNextLevel = MenuItemImage::create(
		"Target-End-Game/Next_Button.png",
		"Target-End-Game/Next_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuNextLevelCallBack, this));
	pNextLevel->setPosition(Point(this->getContentSize().width/2.0f + 115.0f, 370));

	MenuItemImage* pRetryLevel = MenuItemImage::create(
		"Target-End-Game/Retry_Button.png",
		"Target-End-Game/Retry_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuRetryLevelWinCallBack, this));
	pRetryLevel->setPosition(Point(this->getContentSize().width/2.0f - 115.0f, 370));

	Menu* pMenuWin = Menu::create(pRetryLevel, pNextLevel, NULL);
	pMenuWin->setPosition(Point::ZERO);
	this->addChild(pMenuWin);
	
	//Update level data, hackcode chapter
	m_levelInfo = LevelTable::getInstance()->fetchLevel(m_iCurrentLevel);
	std::vector<ChapterInfo> chapters = ChapterTable::getInstance()->getChaptersInfo();
	m_chapterInfo = chapters[m_levelInfo.iChapter-1];

	if (m_levelInfo.bIsUnlock == false)
	{
		m_chapterInfo.iTotalLevelUnlock++;
	}

	m_levelInfo.bIsUnlock = true;
	m_levelInfo.bIsUpdate = true;
	m_levelInfo.sWordKey = pMainWord.m_sWord;
	if (m_levelInfo.iScore < iScore)
	{
		m_levelInfo.iScore = iScore;
	}

	if (m_levelInfo.iBonusQuest < pSubWord.size())
	{
		m_levelInfo.iBonusQuest = pSubWord.size();
	}


	return true;
}

bool EndGameNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(191, 103, 241, 229));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/Close_Button.png",
		"Target-End-Game/Close_Button_Click.png",
		CC_CALLBACK_0(EndGameNode::menuCloseCallBack, this));
	pCloseItem->setPosition(Point(561, 855));

	Menu* pMenu = Menu::create(pCloseItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu, 10);

	m_pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel);
	m_pLeaderBoard->setPosition(Point(320, 136));
	this->addChild(m_pLeaderBoard);

	return true;
}

std::vector<int> EndGameNode::generateArrayNumber(int iNumber)
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

Node* EndGameNode::generateLayoutScore(int iScore)
{
	std::vector<int> arrNumber = this->generateArrayNumber(iScore);
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
	pNode->setAnchorPoint(Point(0.5f, 0.5f));
	pNode->setScale(1.2f);

	return pNode;
}

Node* EndGameNode::generateLayoutStar()
{
	Node* pNode = Node::create();

	Sprite* pBlackStar1 = Sprite::create("Target-End-Game/Star_Disable.png");
	pBlackStar1->setPosition(Point::ZERO);
	pNode->addChild(pBlackStar1);

	Sprite* pBlackStar2 = Sprite::create("Target-End-Game/Star_Disable.png");
	pBlackStar2->setPosition(Point(91, 17));
	pNode->addChild(pBlackStar2);

	Sprite* pBlackStar3 = Sprite::create("Target-End-Game/Star_Disable.png");
	pBlackStar3->setPosition(Point(182, 0));
	pNode->addChild(pBlackStar3);
	
	return pNode;
}

void EndGameNode::addYellowStar(const int& iYellowStar)
{
	m_iYellowStar = iYellowStar;
	m_iCountYellowStar = 0;

	if(m_levelInfo.iStar < iYellowStar)
	{
		m_chapterInfo.iTotalStar += iYellowStar - m_levelInfo.iStar;
		m_levelInfo.iStar = iYellowStar;
	}
	
	ChapterTable::getInstance()->updateChapter(m_chapterInfo);
	LevelTable::getInstance()->updateLevel(m_levelInfo);

	this->sequenceUpdateStar();
}

void EndGameNode::sequenceUpdateStar()
{
	auto actionUpdateStar = CallFunc::create(this, callfunc_selector(EndGameNode::updateStar));
	auto delay = DelayTime::create(0.7f);
	auto actionLoopUpdateStar = CallFunc::create(this, callfunc_selector(EndGameNode::loopUpdateStar));
	this->runAction(Sequence::create(delay->clone(), actionLoopUpdateStar, actionUpdateStar, NULL));
}

void EndGameNode::loopUpdateStar()
{
	if (m_iCountYellowStar < m_iYellowStar)
	{
		this->sequenceUpdateStar();
	}
	else
	{
		this->sequenceUpdateBonusQuest();
	}
}

void EndGameNode::updateStar()
{
	if (m_iCountYellowStar < m_iYellowStar)
	{
		Sprite* pYellowStar = Sprite::create("Target-End-Game/Star_Enable.png");
		if(m_iCountYellowStar == 0)
		{
			pYellowStar->setPosition(Point::ZERO);
		}
		else if(m_iCountYellowStar == 1)
		{
			pYellowStar->setPosition(Point(91, 17));
		}
		else if(m_iCountYellowStar == 2)
		{
			pYellowStar->setPosition(Point(182, 0));
		}

		m_pStarNode->addChild(pYellowStar);
		m_iCountYellowStar++;
	}
}

Node* EndGameNode::generateLayoutBonusQuest()
{
	Node* pNode = Node::create();
	int iWidth = 0;;
	for (int iIndex=0; iIndex<m_iTotalBonusQuest; iIndex++) {
		Sprite* pBonusquest = Sprite::create("Target-End-Game/Bonus_Quest_Disable.png");
		pBonusquest->setPosition(Point(iWidth, 0));
		pNode->addChild(pBonusquest);
		iWidth+= 91;
	}
	
	return pNode;
}

void EndGameNode::addBonusQuestCompleted(const int& iBonusQuestCompleted)
{
	m_iBonusQuestCompleted = iBonusQuestCompleted;
	m_iCountBonusQuest = 0;
}

void EndGameNode::sequenceUpdateBonusQuest()
{
	auto actionUpdateBonusQuest = CallFunc::create(this, callfunc_selector(EndGameNode::updateBonusQuest));
	auto delay = DelayTime::create(0.7f);
	auto actionLoopUpdateBonusQuest = CallFunc::create(this, callfunc_selector(EndGameNode::loopUpdateBonusQuest));
	this->runAction(Sequence::create(delay->clone(), actionLoopUpdateBonusQuest, actionUpdateBonusQuest, NULL));
}

void EndGameNode::loopUpdateBonusQuest()
{
	if (m_iCountBonusQuest < m_iBonusQuestCompleted)
	{
		this->sequenceUpdateBonusQuest();
	}
}

void EndGameNode::updateBonusQuest()
{
	Node* pNode = Node::create();
	int iWidth = 0;;
	for (int iIndex=0; iIndex<m_iTotalBonusQuest; iIndex++) {
		Sprite* pBonusquest = Sprite::create("Target-End-Game/Bonus_Quest_Enable.png");
		pBonusquest->setPosition(Point(iWidth, 0));
		pNode->addChild(pBonusquest);
		m_pBonusQuestNode->addChild(pBonusquest);
		iWidth+= 91;
	}
}


Node* EndGameNode::generateLayoutSubWord(const std::vector<Word>& subWords)
{
	Node* pNode = Node::create();
	int iWidth = 0;
	//while(!subWord.empty())
	for(auto subword : subWords)
	{
		Label* pSubWord = Label::createWithTTF(subword.m_sWord, "fonts/ARLRDBD.ttf", 24);
		pSubWord->setPosition(Point(iWidth, 0));
		pSubWord->setColor(ccc3(0, 0, 0));
		pNode->addChild(pSubWord);

		iWidth += pSubWord->getContentSize().width + 60;
		//subWord.pop_back();
	}

	pNode->setContentSize(CCSizeMake(iWidth-60, 20));
	pNode->setAnchorPoint(Point(0.5f, 0.5f));

	return pNode;
}

Node* EndGameNode::generateLayoutLevel(int iLevel)
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

void EndGameNode::menuNextLevelCallBack()
{
	if ( m_iCurrentLevel >= 8)
		m_iCurrentLevel = 0;

	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(m_iCurrentLevel+1);

	LevelMapScene* pLevelMap = LevelMapScene::create();
	pLevelMap->getLayer()->showPopupTargetGame(m_iCurrentLevel+1);
	CCDirector::getInstance()->replaceScene(pLevelMap);
}

void EndGameNode::menuRetryLevelLoseCallBack()
{
	UserTable::getInstance()->updateLife(0);
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{
		GameWordManager* pGameWordManager = GameWordManager::getInstance();
		pGameWordManager->RetryCurrentLevel();

		CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel);
		CCDirector::getInstance()->replaceScene(pGameScene);
	}
	else
	{
		MessageBox("You have no life!", "Play Level");
	}
}

void EndGameNode::menuRetryLevelWinCallBack()
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->RetryCurrentLevel();

	CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel);
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void EndGameNode::menuCloseCallBack()
{
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}