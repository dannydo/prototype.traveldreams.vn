#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "Database\UserTable.h"
#include "SettingMenuNode.h"
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

	Sprite* pButtonReplayGameSprite = Sprite::create("Target-End-Game/btn_replay.png");
	ButtonNode* buttonReplayNode = ButtonNode::createButtonSprite(pButtonReplayGameSprite, CC_CALLBACK_1(EndGameNode::menuRetryLevelLoseCallBack, this));
	buttonReplayNode->setPosition(Point(320.0f, 302.0f));

	m_pButtonManagerNode->addButtonNode(buttonReplayNode);

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

	Sprite* pCompletedImage = Sprite::createWithSpriteFrameName("complete.png");
	pCompletedImage->setPosition(Point(320.0f, 530.0f));
	m_pSpriteBatchNode->addChild(pCompletedImage);

	Sprite* pButtonNextGameSprite = Sprite::create("Target-End-Game/btn_next.png");
	ButtonNode* buttonNextNode = ButtonNode::createButtonSprite(pButtonNextGameSprite, CC_CALLBACK_1(EndGameNode::menuNextLevelCallBack, this));
	buttonNextNode->setPosition(Point(432.0f, 302.0f));

	Sprite* pButtonReplayGameSprite = Sprite::create("Target-End-Game/btn_replay.png");
	ButtonNode* buttonReplayNode = ButtonNode::createButtonSprite(pButtonReplayGameSprite, CC_CALLBACK_1(EndGameNode::menuRetryLevelWinCallBack, this));
	buttonReplayNode->setPosition(Point(234.0f, 302.0f));

	m_pButtonManagerNode->addButtonNode(buttonNextNode);
	m_pButtonManagerNode->addButtonNode(buttonReplayNode);
	
	//Update level data, hackcode chapter
	m_levelInfo = LevelTable::getInstance()->fetchLevel(m_iCurrentLevel);
	std::vector<ChapterInfo> chapters = ChapterTable::getInstance()->getChaptersInfo();
	m_chapterInfo = chapters[m_levelInfo.iChapter-1];
	UserInfo userInfo = UserTable::getInstance()->getUserInfo();

	if (m_levelInfo.bIsUnlock == false)
	{
		m_chapterInfo.iTotalLevelUnlock++;
		userInfo.iCurrentChapter = m_levelInfo.iChapter;
		userInfo.iCurrentLevel = m_levelInfo.iLevel + 1;
		ChapterTable::getInstance()->updateChapter(m_chapterInfo);
		UserTable::getInstance()->updateUser(userInfo);
	}

	m_levelInfo.bIsUnlock = true;
	m_levelInfo.bIsUpdate = true;
	m_levelInfo.sWordKey = m_mainWord.m_sWord;
	if (m_levelInfo.iScore < m_iScore)
	{
		m_levelInfo.iScore = m_iScore;
	}

	LevelTable::getInstance()->updateLevel(m_levelInfo);

	return true;
}

bool EndGameNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 229));
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

	Sprite* pBackgroundFlashCard = Sprite::createWithSpriteFrameName("flashcard_board.png");
	pBackgroundFlashCard->setPosition(Point(320.0f, 695.0f));
	m_pSpriteBatchNode->addChild(pBackgroundFlashCard);

	Sprite* pScoreImage = Sprite::createWithSpriteFrameName("score.png");
	pScoreImage->setAnchorPoint(Point(0.0f, 0.0f));
	pScoreImage->setPosition(Point(180.0f, 430.0f));
	m_pSpriteBatchNode->addChild(pScoreImage);

	char sScore[10];
	sprintf(sScore, "%d", m_iScore);
	LabelBMFont *pLabelScore = LabelBMFont::create(sScore, "fonts/Score-bitmap-font-game.fnt");
	pLabelScore->setAnchorPoint(Point(0.0f, 0.0f));
	pLabelScore->setPosition(Point(295.0f, 430.0f));
	this->addChild(pLabelScore);

	std::string sPath = "FlashCard/flashcardimage/";
	sPath.append(m_mainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(320.0f, 665.0f));
	pFlashCardImage->setRotation(-3.5);
	this->addChild(pFlashCardImage);

	LabelBMFont *pLabelWord = LabelBMFont::create("PEN", "fonts/Flashcard-bitmap-font-game.fnt");
	pLabelWord->setPosition(Point(320.0f, 760.0f));
	pLabelWord->setRotation(-3.5);
	this->addChild(pLabelWord);	

	Sprite* pLevelImage = Sprite::createWithSpriteFrameName("level.png");
	pLevelImage->setPosition(Point(0.0f, 0.0f));

	char sLevel[10];
	sprintf(sLevel, "%d", m_iCurrentLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/Level-bitmap-font-game.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(pLevelImage->getContentSize().width/2 + pLabelLevel->getContentSize().width/2.0f, 5.0f));
	
	Node* pNodeLevel = Node::create();
	pNodeLevel->addChild(pLabelLevel);
	pNodeLevel->addChild(pLevelImage);
	pNodeLevel->setPosition(Point(320.0f - pLevelImage->getContentSize().width/4.0f - pLabelLevel->getContentSize().width/4.0f + 18, 920.0f));
	this->addChild(pNodeLevel);

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(EndGameNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(582.0f, 914.0f));

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(m_pButtonManagerNode);

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
		
		ChapterTable::getInstance()->updateChapter(m_chapterInfo);
		LevelTable::getInstance()->updateLevel(m_levelInfo);
	}

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
		LevelTable::getInstance()->updateLevel(m_levelInfo);
	}

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

void EndGameNode::menuNextLevelCallBack(Object* sender)
{
	if ( m_iCurrentLevel >= 30)
		m_iCurrentLevel = 0;

	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(m_iCurrentLevel+1);

	LevelMapScene* pLevelMap = LevelMapScene::create();
	pLevelMap->getLayer()->showPopupTargetGame(m_iCurrentLevel+1);
	CCDirector::getInstance()->replaceScene(pLevelMap);
}

void EndGameNode::menuRetryLevelLoseCallBack(Object* sender)
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

void EndGameNode::menuRetryLevelWinCallBack(Object* sender)
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->RetryCurrentLevel();

	CCScene *pGameScene = HelloWorld::createScene(m_iCurrentLevel);
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void EndGameNode::menuCloseCallBack(Object* sender)
{
	Breadcrumb::getInstance()->getSceneModePopBack();
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}