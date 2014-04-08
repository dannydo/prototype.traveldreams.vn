#include "EndGameNode.h"
#include "LevelMapScene.h"
#include "HelloWorldScene.h"
#include "Database\UserTable.h"
#include "SettingMenuNode.h"
#include "Database\UserTable.h"
#include "Database\VersionTable.h"
#include "GameConfigManager.h"
#include "Database\InitDatabase.h"
#include "Database\WordTable.h"
#include "WorldMapScene.h"

USING_NS_CC;

EndGameNode* EndGameNode::createLayoutLose(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId)
{
	EndGameNode* pEndGameNode = new EndGameNode();
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	pEndGameNode->m_mainWord = mainWord;
	pEndGameNode->m_iScore = iScore;
	pEndGameNode->m_sChapterId = sChapterId;

	if(pEndGameNode->initLose())
	{
		pEndGameNode->autorelease();
		return pEndGameNode;
	}

	CC_SAFE_DELETE(pEndGameNode);
	return NULL;
}

EndGameNode* EndGameNode::createLayoutWin(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId)
{
	EndGameNode* pEndGameNode = new EndGameNode();	  
	pEndGameNode->m_iCurrentLevel = iCurrentLevel;
	pEndGameNode->m_mainWord = mainWord;
	pEndGameNode->m_iScore = iScore;
	pEndGameNode->m_sChapterId = sChapterId;

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

	Sprite* pMonsterImage = Sprite::createWithSpriteFrameName("pet-fail-r.PNG");
	pMonsterImage->setPosition(Point(548.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pMonsterImage);

	Sprite* pPetImage = Sprite::createWithSpriteFrameName("pet-fail-l.png");
	pPetImage->setPosition(Point(102.0f, 378.0f));
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

	LevelConfig* pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig(m_sChapterId, m_iCurrentLevel);
	m_iTotalBonusQuest = pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount;

	this->generateLayoutStartAndBonusQuest();

	
	Sprite* pMonsterImage = Sprite::createWithSpriteFrameName("pet-r.png");
	pMonsterImage->setPosition(Point(548.0f, 378.0f));
	m_pSpriteBatchNode->addChild(pMonsterImage);

	Sprite* pPetImage = Sprite::createWithSpriteFrameName("pet-l.png");
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
	
	m_levelInfo = LevelTable::getInstance()->getLevel(m_sChapterId, m_iCurrentLevel);
	m_chapterInfo = ChapterTable::getInstance()->getChapterInfo(m_sChapterId);

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();

	// Van Dao
	if (m_levelInfo.bIsUnlock == false)
	{
		m_chapterInfo.iTotalLevelUnlock++;

		userInfo.sCurrentChapterId = m_levelInfo.sChapterId;
		userInfo.iCurrentLevel = m_levelInfo.iLevel + 1;

		// Unlock chapter
		WordlMapConfig worldMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
		int iIndexCurrentChapter = worldMapConfig.m_WorlMapChapterConfigMap[m_levelInfo.sChapterId];
		WordlMapConfig::WordMapChapterConfig worldMapChapterConfig = worldMapConfig.m_WorlMapChapterConfigs[iIndexCurrentChapter];

		if (m_iCurrentLevel == worldMapChapterConfig.m_iTotalevel)
		{
			// Create data for one chapter
			std::vector<std::string> wordList;
			std::vector<int> mapLevels;

			std::string sNextChapterID;
			if ( GameConfigManager::getInstance()->GetNextChapterID(worldMapChapterConfig.m_sChapterId, sNextChapterID))
			{
				GameConfigManager::getInstance()->GenerateWordsForLevels( sNextChapterID, wordList, mapLevels);
				InitDatabase::getInstance()->createDataChapterAndLevel( sNextChapterID, wordList, mapLevels);
				ChapterTable::getInstance()->refreshChapters();

				userInfo.sCurrentChapterId = sNextChapterID;
				userInfo.iCurrentLevel = 1;
				UserDefault::getInstance()->setStringForKey("ChapterPlayGame", userInfo.sCurrentChapterId);
			}
			else // the last chapter is finished so game is end now!!!!
			{
				
			}
		}
		
		UserTable::getInstance()->updateUser(userInfo);

		m_levelInfo.bIsUnlock = true;
		m_levelInfo.iTotalBonusQuest = m_iTotalBonusQuest;
	}
	
	if (m_levelInfo.iScore < m_iScore)
	{
		m_levelInfo.iScore = m_iScore;
	}

	LevelTable::getInstance()->updateLevel(m_levelInfo);

	// Update Word for chapter
	const std::string sWordId = GameWordManager::getInstance()->GetWordIdFromWord(m_mainWord);
	WordInfo wordInfo = WordTable::getInstance()->getWordInfoOnChapter(m_levelInfo.sChapterId, sWordId);

	if (wordInfo.bIsNew == false && wordInfo.iCountCollected <= 1)
	{
		m_chapterInfo.iCountFlashCardNew++;
		m_chapterInfo.iTotalFlashCardUnlock++;

		wordInfo.bIsNew = true;
		wordInfo.iOrderUnlock = m_chapterInfo.iTotalFlashCardUnlock;
	}

	wordInfo.iCountCollected++;
	WordTable::getInstance()->updateWord(wordInfo);
	ChapterTable::getInstance()->updateChapter(m_chapterInfo);

	return true;
}

bool EndGameNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
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

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(m_mainWord);;
	sPath.append("/FlashCard/");
	sPath.append(m_mainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(320.0f, 665.0f));
	pFlashCardImage->setRotation(-3.5);
	this->addChild(pFlashCardImage);

	LabelBMFont *pLabelWord = LabelBMFont::create(m_mainWord.m_sWord, "fonts/Flashcard-bitmap-font-game.fnt");
	pLabelWord->setPosition(Point(320.0f, 760.0f));
	pLabelWord->setRotation(-3.5);
	this->addChild(pLabelWord);	

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
	pNodeLevel->setPosition(Point(320.0f - pLevelImage->getContentSize().width/4.0f - pLabelLevel->getContentSize().width/4.0f + 18, 920.0f));
	this->addChild(pNodeLevel);

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(EndGameNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(582.0f, 914.0f));

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(m_pButtonManagerNode);

	m_pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel, m_sChapterId);
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
	if (m_iCountYellowStar < m_iYellowStar) {
		this->runAction(Sequence::create(delay->clone(), actionUpdateStar, NULL));
	}
	else
	{
		this->sequenceUpdateBonusQuest();
	}
}

void EndGameNode::updateStar()
{
	if (m_iCountYellowStar < m_iYellowStar) {
		Sprite* pStarYellowImage = Sprite::createWithSpriteFrameName("star_yellow.png");
		pStarYellowImage->setPosition(Point(200.0f + m_iCountYellowStar*108.0f - m_iTotalBonusQuest*46, 823.0f + m_iCountYellowStar*5 - m_iTotalBonusQuest*5));
		m_pSpriteBatchNode->addChild(pStarYellowImage);
		m_iCountYellowStar++;
		this->sequenceUpdateStar();
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
	auto delay = DelayTime::create(0.5f);
	this->runAction(Sequence::create(delay->clone(), actionUpdateBonusQuest, NULL));
}

void EndGameNode::updateBonusQuest()
{
	if (m_iCountBonusQuest < m_iBonusQuestCompleted && m_iCountBonusQuest < m_iTotalBonusQuest)
	{
		Sprite* pMushRoom = Sprite::createWithSpriteFrameName("mushroom.png");
		pMushRoom->setPosition(Point(529.0f + m_iCountBonusQuest*98.0f - m_iTotalBonusQuest*50, 835.0f + m_iCountBonusQuest*5 + 3));
		m_pSpriteBatchNode->addChild(pMushRoom);
		m_iCountBonusQuest++;
		this->sequenceUpdateBonusQuest();
	}
}

void EndGameNode::menuNextLevelCallBack(Object* sender)
{
	Breadcrumb::getInstance()->getSceneModePopBack();
	if ( m_iCurrentLevel >= _MAX_GAME_LEVEL_)
		m_iCurrentLevel = 0;


	ChapterConfig& currentChapter = GameConfigManager::getInstance()->GetChapterConfig(m_sChapterId);
	if (currentChapter.m_iTotalevel <= m_iCurrentLevel)
	{
		std::string sNextChapterID;
		if ( GameConfigManager::getInstance()->GetNextChapterID( m_sChapterId, sNextChapterID))
		{
			ChapterConfig& nextChapter = GameConfigManager::getInstance()->GetChapterConfig(sNextChapterID);	
			m_sChapterId = sNextChapterID;
			m_iCurrentLevel = 1;

			WorldMapScene* pWorldMap = WorldMapScene::create();
			CCDirector::getInstance()->replaceScene(pWorldMap);
			return;
		}
		else // all level/chapter is finished
		{
			Breadcrumb::getInstance()->getSceneModePopBack();
			WorldMapScene* pWorldMap = WorldMapScene::create();
			CCDirector::getInstance()->replaceScene(pWorldMap);
			return;
		}
	}
	else
		m_iCurrentLevel++;
	
	// already callled in showPopupTargetGame method
	//GameWordManager* pGameWordManager = GameWordManager::getInstance();
	//pGameWordManager->GenerateWordForNewLevel(m_sChapterId, m_iCurrentLevel);

	LevelMapScene* pLevelMap = LevelMapScene::create();
	pLevelMap->getLayer()->showPopupTargetGame(m_sChapterId, m_iCurrentLevel);
	CCDirector::getInstance()->replaceScene(pLevelMap);
}

void EndGameNode::menuRetryLevelLoseCallBack(Object* sender)
{
	UserTable::getInstance()->updateLife(0);
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{
		GameWordManager* pGameWordManager = GameWordManager::getInstance();
		pGameWordManager->RetryCurrentLevel();

		CCScene *pGameScene = HelloWorld::createScene();
		CCDirector::getInstance()->replaceScene(pGameScene);
	}
	else
	{
		MessageBox("You have no life!", "Play Level");
	}
}

void EndGameNode::menuRetryLevelWinCallBack(Object* sender)
{
	// make sure we already cache correct current chapter/level 
	GameConfigManager::getInstance()->GetLevelConfig( m_sChapterId, m_iCurrentLevel);
	

	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel( m_sChapterId, m_iCurrentLevel);

	CCScene *pGameScene = HelloWorld::createScene();
	CCDirector::getInstance()->replaceScene(pGameScene);
}

void EndGameNode::menuCloseCallBack(Object* sender)
{
	Breadcrumb::getInstance()->getSceneModePopBack();
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}