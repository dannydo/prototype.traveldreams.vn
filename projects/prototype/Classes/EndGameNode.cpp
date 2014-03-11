#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "DictionaryNode.h"
#include "Database\UserTable.h"

USING_NS_CC;

EndGameNode* EndGameNode::createLayoutLose(const int& iScore, const Word& mainWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	pEndGameNode->m_mainWord = mainWord;
	pEndGameNode->m_iScore = iScore;
	if(pEndGameNode->initLose())
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

EndGameNode* EndGameNode::createLayoutWin(const int& iScore, const Word& mainWord, const int& iCurrentLevel)
{
	EndGameNode* pEndGameNode = new EndGameNode();	  
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	pEndGameNode->m_mainWord = mainWord;
	pEndGameNode->m_iScore = iScore;
	if(pEndGameNode->initWin())
	{			
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

bool EndGameNode::initLose()
{
	if (!this->init())
	{
		return false;
	}

	Sprite* pLevelFailImage3 = Sprite::createWithSpriteFrameName("fail.png");
	pLevelFailImage3->setPosition(Point(320.0f, 830.0f));
	m_pSpriteBatchNode->addChild(pLevelFailImage3);

	Sprite* pSenetnceFailImage3 = Sprite::createWithSpriteFrameName("didnt collect card.png");
	pSenetnceFailImage3->setPosition(Point(320.0f, 535.0f));
	m_pSpriteBatchNode->addChild(pSenetnceFailImage3);

	Sprite* pPetImage = Sprite::createWithSpriteFrameName("pet-fail.png");
	pPetImage->setPosition(Point(135.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pPetImage);

	MenuItemImage* pRetryLevel = MenuItemImage::create(
		"Target-End-Game/btn_replay.png",
		"Target-End-Game/btn_replay.png",
		CC_CALLBACK_0(EndGameNode::menuRetryLevelLoseCallBack, this));

	Menu* pMenuLose = Menu::create(pRetryLevel, NULL);
	pMenuLose->setPosition(Point(320.0f, 302.0f));
	this->addChild(pMenuLose);

	return true;
}

bool EndGameNode::initWin()
{
	if (!this->init())
	{
		return false;
	}

	LevelConfig* pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig(m_iCurrentLevel);
	m_iTotalBonusQuest = pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount;

	this->generateLayoutStartAndBonusQuest();

	Sprite* pPetImage = Sprite::createWithSpriteFrameName("pet.png");
	pPetImage->setPosition(Point(102.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pPetImage);

	MenuItemImage* pNextLevel = MenuItemImage::create(
		"Target-End-Game/btn_next.png",
		"Target-End-Game/btn_next.png",
		CC_CALLBACK_0(EndGameNode::menuNextLevelCallBack, this));
	pNextLevel->setPosition(Point(432.0f, 302.0f));

	MenuItemImage* pRetryLevel = MenuItemImage::create(
		"Target-End-Game/btn_replay.png",
		"Target-End-Game/btn_replay.png",
		CC_CALLBACK_0(EndGameNode::menuRetryLevelWinCallBack, this));
	pRetryLevel->setPosition(Point(234.0f, 302.0f));

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
	m_levelInfo.sWordKey = m_mainWord.m_sWord;
	if (m_levelInfo.iScore < m_iScore)
	{
		m_levelInfo.iScore = m_iScore;
	}

	ChapterTable::getInstance()->updateChapter(m_chapterInfo);
	LevelTable::getInstance()->updateLevel(m_levelInfo);

	return true;
}

bool EndGameNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(101, 85, 130, 255));
	pBackground->setContentSize(CCSizeMake(640, 960));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
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

	Sprite* pScoreImage = Sprite::createWithSpriteFrameName("score.png");
	pScoreImage->setAnchorPoint(Point(0.0f, 0.0f));
	pScoreImage->setPosition(Point(180.0f, 430.0f));
	m_pSpriteBatchNode->addChild(pScoreImage);

	char sScore[10];
	sprintf(sScore, "%d", m_iScore);
	LabelTTF* pLabelScore = LabelTTF::create(sScore, "Arial", 40);
	pLabelScore->setColor(ccc3(220, 143, 21));
	pLabelScore->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelScore->setPosition(Point(295.0f, 430.0f));
	this->addChild(pLabelScore);

	std::string sPath = "FlashCard/flashcardimage/";
	sPath.append(m_mainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(320.0f, 680.0f));
	pFlashCardImage->setRotation(-3.5);
	this->addChild(pFlashCardImage);

	Sprite* pTargetImage = Sprite::create("Target-End-Game/target.png");
	pTargetImage->setPosition(Point(320.0f, 755.0f));
	pTargetImage->setRotation(-3.5);
	this->addChild(pTargetImage);

	LabelTTF* pLabelWord = LabelTTF::create(m_mainWord.m_sWord, "Arial", 25);
	pLabelWord->setPosition(Point(320.0f, 720.0f));
	pLabelWord->setRotation(-3.5);
	this->addChild(pLabelWord);

	char sLevel[10];
	sprintf(sLevel, "%d", m_iCurrentLevel);
	LabelTTF* pLabelLevel = LabelTTF::create(sLevel, "Arial", 32);
	this->addChild(pLabelLevel);

	int iWidth = pLabelLevel->getContentSize().width+pLevelImage->getContentSize().width;
	pLabelLevel->setPosition(Point(320.0f+iWidth/4.0f+iWidth/4.0f, 920.0f));
	pLevelImage->setPosition(Point(320.0f+iWidth/4.0f-iWidth/4.0f, 920.0f));

	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Target-End-Game/btn_close.png",
		"Target-End-Game/btn_close.png",
		CC_CALLBACK_0(EndGameNode::menuCloseCallBack, this));
	pCloseItem->setPosition(Point(582.0f, 914.0f));

	Menu* pMenu = Menu::create(pCloseItem, NULL);
	pMenu->setPosition(Point::ZERO);
	this->addChild(pMenu, 10);

	m_pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel);
	m_pLeaderBoard->setPosition(Point(320, 114));
	this->addChild(m_pLeaderBoard);

	return true;
}

void EndGameNode::generateLayoutStartAndBonusQuest()
{
	for(int iIndex=0; iIndex<3; iIndex++) 
	{
		Sprite* pStarPurpleImage = Sprite::createWithSpriteFrameName("star_purple.png");
		pStarPurpleImage->setPosition(Point(200.0f + iIndex*108.0f - m_iTotalBonusQuest*46, 823.0f + iIndex*5 - m_iTotalBonusQuest*5));
		m_pSpriteBatchNode->addChild(pStarPurpleImage);
	}

	for(int iIndex=0; iIndex<m_iTotalBonusQuest; iIndex++) 
	{
		Sprite* pMushRoomFail = Sprite::createWithSpriteFrameName("mushroom-fail.PNG");
		pMushRoomFail->setPosition(Point(529.0f + iIndex*98.0f - m_iTotalBonusQuest*50, 835.0f + iIndex*5 + 3));
		m_pSpriteBatchNode->addChild(pMushRoomFail);
	}
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
	auto delay = DelayTime::create(0.5f);
	auto actionLoopUpdateStar = CallFunc::create(this, callfunc_selector(EndGameNode::loopUpdateStar));
	if (m_iCountYellowStar < m_iYellowStar-1) {
		this->runAction(Sequence::create(delay->clone(), actionLoopUpdateStar, actionUpdateStar, NULL));
	}
	else
	{
		this->sequenceUpdateBonusQuest();
	}
}

void EndGameNode::loopUpdateStar()
{
	if (m_iCountYellowStar < m_iYellowStar)
	{
		this->sequenceUpdateStar();
	}
}

void EndGameNode::updateStar()
{
	if (m_iCountYellowStar < m_iYellowStar) {
		Sprite* pStarYellowImage = Sprite::createWithSpriteFrameName("star_yellow.png");
		pStarYellowImage->setPosition(Point(200.0f + m_iCountYellowStar*108.0f - m_iTotalBonusQuest*46, 823.0f + m_iCountYellowStar*5 - m_iTotalBonusQuest*5));
		m_pSpriteBatchNode->addChild(pStarYellowImage);
		m_iCountYellowStar++;
	}
}

void EndGameNode::addBonusQuestCompleted(const int& iBonusQuestCompleted)
{
	if(m_levelInfo.iBonusQuest < iBonusQuestCompleted)
	{
		m_levelInfo.iBonusQuest = iBonusQuestCompleted;
	}

	m_iBonusQuestCompleted = iBonusQuestCompleted;
	m_iCountBonusQuest = 0;
	LevelTable::getInstance()->updateLevel(m_levelInfo);
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
	if (m_iCountBonusQuest < m_iBonusQuestCompleted && m_iCountBonusQuest < m_iTotalBonusQuest)
	{
		this->sequenceUpdateBonusQuest();
	}
}

void EndGameNode::updateBonusQuest()
{
	if (m_iCountBonusQuest < m_iBonusQuestCompleted && m_iCountBonusQuest < m_iTotalBonusQuest)
	{
		Sprite* pMushRoom = Sprite::createWithSpriteFrameName("mushroom.png");
		pMushRoom->setPosition(Point(529.0f + m_iCountBonusQuest*98.0f - m_iTotalBonusQuest*50, 835.0f + m_iCountBonusQuest*5 + 3));
		m_pSpriteBatchNode->addChild(pMushRoom);
		m_iCountBonusQuest++;
	}
}

void EndGameNode::menuNextLevelCallBack()
{
	if ( m_iCurrentLevel >= _MAX_GAME_LEVEL_)
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