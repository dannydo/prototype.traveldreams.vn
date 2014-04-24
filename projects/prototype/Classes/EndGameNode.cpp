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
#include "APIService\SyncDataGame.h"
#include "FunctionCommon.h"

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

bool EndGameNode::initWin()
{
	if (!Node::init())
	{
		return false;
	}

	LevelConfig* pLevelConfig = &GameConfigManager::getInstance()->GetLevelConfig(m_sChapterId, m_iCurrentLevel);
	m_iTotalBonusQuest = pLevelConfig->m_BonusQuestConfig.m_iBonusQuestCount;

	this->generateLayoutStartAndBonusQuest();

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	//m_pSpriteBatchNode = SpriteBatchNode::create("Target-End-Game/TargetEndgame.pvr.ccz");
	//SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Target-End-Game/TargetEndgame.plist");
	//this->addChild(m_pSpriteBatchNode);

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pCompletedImage = Sprite::create("Target-End-Game/text_level_completed.png");
	pCompletedImage->setPosition(Point(320.0f, 654.0f));
	this->addChild(pCompletedImage);

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(m_mainWord);;
	sPath.append("/FlashCard/");
	sPath.append(m_mainWord.m_sFlashCardImage);
	Sprite* pFlashCardImage = Sprite::create(sPath.c_str());
	pFlashCardImage->setPosition(Point(240.0f, 495.0f));
	this->addChild(pFlashCardImage);

	char sLevel[20];
	int iCalLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId);
	sprintf(sLevel, "Level %d", m_iCurrentLevel + iCalLevel);
	LabelBMFont *pLabelLevel = LabelBMFont::create(sLevel, "fonts/font-bechic.fnt");
	pLabelLevel->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLevel->setPosition(Point(320.0f, 870.0f));
	this->addChild(pLabelLevel);
	
	LabelBMFont *pLabelScore = LabelBMFont::create("SCORE:", "fonts/font_score.fnt");
	pLabelScore->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelScore->setPosition(Point(474.0f, 525.0f));
	this->addChild(pLabelScore);

	LabelBMFont *pLabelNumScore = LabelBMFont::create(formatNumber(m_iScore).getCString(), "fonts/font_score.fnt");
	pLabelNumScore->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelNumScore->setPosition(Point(474.0f, 480.0f));
	pLabelNumScore->setScale(1.4f);
	this->addChild(pLabelNumScore);

	LeaderBoardtNode* pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel, m_sChapterId);
	pLeaderBoard->setPosition(Point(320.0f, 114.0f));
	this->addChild(pLeaderBoard);

	Sprite* pButtonNextGameSprite = Sprite::create("Target-End-Game/btn_next.png");
	ButtonNode* buttonNextNode = ButtonNode::createButtonSprite(pButtonNextGameSprite, CC_CALLBACK_1(EndGameNode::menuNextLevelCallBack, this));
	buttonNextNode->setPosition(Point(438.0f, 279.0f));

	Sprite* pButtonReplayGameSprite = Sprite::create("Target-End-Game/btn_replay.png");
	ButtonNode* buttonReplayNode = ButtonNode::createButtonSprite(pButtonReplayGameSprite, CC_CALLBACK_1(EndGameNode::menuRetryLevelWinCallBack, this));
	buttonReplayNode->setPosition(Point(225.0f, 279.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(EndGameNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonNextNode);
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	pButtonManagerNode->addButtonNode(buttonReplayNode);
	this->addChild(pButtonManagerNode);
	
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
			// Create data for new chapter
			std::vector<std::string> wordList;
			std::vector<int> mapLevels;

			std::string sNextChapterID;
			if ( GameConfigManager::getInstance()->GetNextChapterID(worldMapChapterConfig.m_sChapterId, sNextChapterID))
			{
				GameConfigManager::getInstance()->GenerateWordsForNewChapter( sNextChapterID, wordList, mapLevels);
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

bool EndGameNode::initLose()
{
	if (!Node::init())
	{
		return false;
	}
	
	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event* event) { return true;  };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, pBackground);
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	//m_pSpriteBatchNode = SpriteBatchNode::create("Target-End-Game/TargetEndgame.pvr.ccz");
	//SpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Target-End-Game/TargetEndgame.plist");
	//this->addChild(m_pSpriteBatchNode);

	Sprite* pBackgroundBoard = Sprite::create("Target-End-Game/panel-level_popup.png");
	pBackgroundBoard->setPosition(Point(320.0f, 610.0f));
	this->addChild(pBackgroundBoard);

	Sprite* pLevelFailImage3 = Sprite::create("Target-End-Game/text_level_fail.png");
	pLevelFailImage3->setPosition(Point(320.0f, 750.0f));
	this->addChild(pLevelFailImage3);

	Sprite* pIconBoosterImage = Sprite::create("Target-End-Game/icon-boosters.png");
	pIconBoosterImage->setPosition(Point(320.0f, 437.0f));
	this->addChild(pIconBoosterImage);

	std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(m_mainWord);;
	sPath.append("/FlashCard/");
	sPath.append(m_mainWord.m_sFlashCardImage);
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
	
	LabelBMFont *pLabelScore = LabelBMFont::create("SCORE:", "fonts/font_score.fnt");
	pLabelScore->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelScore->setPosition(Point(474.0f, 630.0f));
	this->addChild(pLabelScore);

	LabelBMFont *pLabelNumScore = LabelBMFont::create(formatNumber(m_iScore).getCString(), "fonts/font_score.fnt");
	pLabelNumScore->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelNumScore->setPosition(Point(474.0f, 585.0f));
	pLabelNumScore->setScale(1.4f);
	this->addChild(pLabelNumScore);

	LeaderBoardtNode* pLeaderBoard = LeaderBoardtNode::createLayout(m_iCurrentLevel, m_sChapterId);
	pLeaderBoard->setPosition(Point(320.0f, 114.0f));
	this->addChild(pLeaderBoard);

	Sprite* pButtonPlayGameSprite = Sprite::create("Target-End-Game/btn_replay.png");
	ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonPlayGameSprite, CC_CALLBACK_1(EndGameNode::menuRetryLevelLoseCallBack, this));
	buttonPlayNode->setPosition(Point(320.0f, 279.0f));

	Sprite* pButtonCloseSprite = Sprite::create("Target-End-Game/btn_close.png");
	ButtonNode* buttonCloseNode = ButtonNode::createButtonSprite(pButtonCloseSprite, CC_CALLBACK_1(EndGameNode::menuCloseCallBack, this));
	buttonCloseNode->setPosition(Point(572.0f, 894.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(buttonPlayNode);
	pButtonManagerNode->addButtonNode(buttonCloseNode);
	this->addChild(pButtonManagerNode);

	return true;
}

void EndGameNode::generateLayoutStartAndBonusQuest()
{
	for(int iIndex=0; iIndex<3; iIndex++) 
	{
		Sprite* pStarPurpleImage = Sprite::create("Target-End-Game/star_purple.png");
		pStarPurpleImage->setPosition(Point(210.0f + iIndex*108.0f - m_iTotalBonusQuest*46, 750.0f));
		this->addChild(pStarPurpleImage);
	}

	for(int iIndex=0; iIndex<m_iTotalBonusQuest; iIndex++) 
	{
		Sprite* pMushRoomFail = Sprite::create("Target-End-Game/mushroom-fail.png");
		pMushRoomFail->setPosition(Point(539.0f + iIndex*98.0f - m_iTotalBonusQuest*50, 750.0f));
		this->addChild(pMushRoomFail);
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

	SyncDataGame::getInstance()->runSyncDataGame();

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
		Sprite* pStarYellowImage = Sprite::create("Target-End-Game/star_yellow.png");
		pStarYellowImage->setPosition(Point(210.0f + m_iCountYellowStar*108.0f - m_iTotalBonusQuest*46, 750.0f));
		this->addChild(pStarYellowImage);
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
		Sprite* pMushRoom = Sprite::create("Target-End-Game/mushroom.png");
		pMushRoom->setPosition(Point(539.0f + m_iCountBonusQuest*98.0f - m_iTotalBonusQuest*50, 750.0f));
		this->addChild(pMushRoom);
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