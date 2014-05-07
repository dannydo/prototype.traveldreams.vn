#include "LevelMapScene.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "GameTargetNode.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "FlashCardCollection.h"
#include "Database\UserTable.h"
#include "HeaderNode.h"
#include "Database\UserTable.h"	 
#include "Config.h"
#include "GameConfigManager.h"
#include "Database\WordTable.h"
#include "GetMoreLifeNode.h"
#include "EndGameNode.h"
#include "QuitLevelFailedNode.h"

using namespace cocos2d;

bool LevelMapScene::init()
{
	if(CCScene::init())
	{
		this->_layer = LevelMapLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

LevelMapScene::~LevelMapScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

LevelMapLayer::~LevelMapLayer()
{
	m_pButtonManagerNode = NULL;	
}

bool LevelMapLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

	m_sChapterId = UserDefault::getInstance()->getStringForKey("ChapterPlayGame");
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	ChapterConfig chapterConfig = GameConfigManager::getInstance()->GetChapterConfig(m_sChapterId);
	WordlMapConfig::WordMapChapterConfig wordMapChapterConfig =  GameConfigManager::getInstance()->GetWordMapChapterConfig(m_sChapterId);

	m_pBackgroundNode = Node::create();
	m_pBackgroundNode->setAnchorPoint(Point(0.5f, 0.0f));
	m_pBackgroundNode->setPosition(Point(0.0f, 0.0f));
	this->addChild(m_pBackgroundNode);
	m_maxHeight=0;

	std::string sPathFile;
	for(int iIndex=1; iIndex<=chapterConfig.m_iTotalBackgroundImage; iIndex++)
	{
		char sBGChapter[10];
		sprintf(sBGChapter, "/bg%d.png", iIndex);

		sPathFile = wordMapChapterConfig.m_sPathData;
		sPathFile.append(sBGChapter);

		Sprite* pBGImage = Sprite::create(sPathFile.c_str());
		pBGImage->setAnchorPoint(Point(0.5f, 0.0f));
		pBGImage->setPosition(Point(320.0f, m_maxHeight));

		m_pBackgroundNode->addChild(pBGImage);
		m_maxHeight += pBGImage->getContentSize().height;
	}

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->AllowSwipingBackground(true);
	m_pBackgroundNode->addChild(m_pButtonManagerNode);

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	Point pointScroll;
	m_levels = LevelTable::getInstance()->getAllLevelsForChapter(m_sChapterId);
	
	m_iCalculatorLevel = GameConfigManager::getInstance()->CountLevelOfPreviousChapters(m_sChapterId);
	for(int iIndex=0; iIndex<chapterConfig.m_iTotalevel; iIndex++)
	{
		LevelInfo levelInfo = m_levels[iIndex];
		Point point = chapterConfig.m_PositionLevel[iIndex];

		char sLevel[10];
		sprintf(sLevel, "%d", levelInfo.iLevel + m_iCalculatorLevel);
		LabelBMFont *pLevelLabel = LabelBMFont::create(sLevel, "fonts/font_title-popup.fnt");
		pLevelLabel->setScale(0.75f);
		pLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
		pLevelLabel->setPosition(Point(74.0f, 60.0f));
		
		if(UserDefault::getInstance()->getIntegerForKey("IsUnlockALlLevel", 0) == 1 // for test
			|| levelInfo.bIsUnlock || (levelInfo.iLevel == userInfo.iCurrentLevel && levelInfo.sChapterId == userInfo.sCurrentChapterId))
		{
			m_ButtonNextLevelSprite = NULL;

			if(levelInfo.iLevel == userInfo.iCurrentLevel && levelInfo.sChapterId == userInfo.sCurrentChapterId)
			{
				pointScroll = point;

				Sprite* pButtonLevelSprite = Sprite::create("World-Map/new-level.png");
				pButtonLevelSprite->addChild(pLevelLabel);

				m_pButtonNodeCurrentLevel = ButtonNode::createButtonSprite(pButtonLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
				m_pButtonNodeCurrentLevel->setPosition(point);
				m_pButtonNodeCurrentLevel->setTag(levelInfo.iLevel);
				m_pButtonManagerNode->addButtonNode(m_pButtonNodeCurrentLevel);
			}
			else
			{
				Sprite* pButtonLevelSprite = Sprite::create("World-Map/pass-level.png");
				pButtonLevelSprite->addChild(pLevelLabel);

				ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
				buttonPlayNode->setPosition(point);
				buttonPlayNode->setTag(levelInfo.iLevel);
				m_pButtonManagerNode->addButtonNode(buttonPlayNode);

				/*
				WordInfo wordInfo = WordTable::getInstance()->getWordInfoOnChapter(levelInfo.sChapterId, levelInfo.sWordId);
				if(wordInfo.iCountCollected <= 1)
				{
					LabelTTF* pLabelNew = LabelTTF::create("New", "Arial", 22);
					pLabelNew->setColor(ccc3(255.0f, 0.0f, 0.0f));
					pLabelNew->setAnchorPoint(Point(0.5f, 0.5f));
					pLabelNew->setPosition(Point(point.x-2, point.y-8));
					m_pBackgroundNode->addChild(pLabelNew);
				}
				*/
			}

			if (levelInfo.bIsUnlock || (levelInfo.iLevel == userInfo.iCurrentLevel && levelInfo.sChapterId == userInfo.sCurrentChapterId))
			{
				Node* pStarAndBonusQuestNode = this->generateLayoutStarAndBonusQuest(levelInfo.iStar, levelInfo.iBonusQuest, levelInfo.iTotalBonusQuest);
				pStarAndBonusQuestNode->setPosition(Point(point.x, point.y - 45));
				m_pBackgroundNode->addChild(pStarAndBonusQuestNode);
			}
		}
		else
		{
			if(levelInfo.iLevel == userInfo.iCurrentLevel+1 && levelInfo.sChapterId == userInfo.sCurrentChapterId)
			{
				m_ButtonNextLevelSprite = Sprite::create("World-Map/Level_lock.png");
				m_ButtonNextLevelSprite->setPosition(point);
				m_ButtonNextLevelSprite->setTag(levelInfo.iLevel);
				m_pBackgroundNode->addChild(m_ButtonNextLevelSprite);
			}
			else
			{
				Sprite* pButtonLevelSprite = Sprite::create("World-Map/Level_lock.png");
				pButtonLevelSprite->setPosition(point);
				m_pBackgroundNode->addChild(pButtonLevelSprite);
			}
		}
	}

	if (pointScroll.y > 960-94-200 && pointScroll.y < m_maxHeight-960) {
		m_pBackgroundNode->setPositionY(-pointScroll.y + 550);
	}
	else if(pointScroll.y > m_maxHeight-960)
	{
		m_pBackgroundNode->setPositionY(-(m_maxHeight-960));
	}

	m_pFooterNode = FooterNode::create();
	this->addChild(m_pFooterNode);
	
	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kLevelMap);

	HeaderNode* pHeaderNode = HeaderNode::create();
	this->addChild(pHeaderNode);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	m_pScrollManager = new ScrollManager();
	m_bIsSwipe = false;

	return true;
}

void LevelMapLayer::menuLevelSelected(CCObject* pSender)
{
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{		
		// play sound effect
		SoundManager::PlaySoundEffect(_SET_LEVEL_SELECT_);

		ButtonNode* pButtonNode = (ButtonNode*)pSender;
		int iLevel = pButtonNode->getTag();

		GameWordManager* pGameWordManager = GameWordManager::getInstance( );
		pGameWordManager->GenerateWordForNewLevel(m_sChapterId, iLevel);		

		GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), iLevel, m_sChapterId);
		this->addChild(pGameTargetNode, 10);		
	}
	else
	{
		GetMoreLifeNode* pGetMoreLife = GetMoreLifeNode::create();
		pGetMoreLife->setGetMoreLifeType(GetMoreLifeType::eClosePopup);
		this->addChild(pGetMoreLife);
	}
}

void LevelMapLayer::showPopupEndGameLose(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId)
{
	EndGameNode* pEndGameLose = EndGameNode::createLayoutLose(iScore, mainWord, iCurrentLevel, sChapterId);
	this->addChild(pEndGameLose, 10);
}

void LevelMapLayer::showPopupEndGameWin(const int& iScore, const Word& mainWord, const int& iCurrentLevel, const std::string sChapterId, const int& iYellowStar, const int& iBonusQuestCompleted)
{
	EndGameNode* pEndGameLose = EndGameNode::createLayoutWin(iScore, mainWord, iCurrentLevel, sChapterId, iYellowStar, iBonusQuestCompleted);
	this->addChild(pEndGameLose, 10);
}

void LevelMapLayer::showPopupQuitLevelFailed( const int& iCurrentLevel, const std::string sChapterId)
{
	QuitLevelFailedNode* pQuitLevelFailed = QuitLevelFailedNode::createLayout(iCurrentLevel, sChapterId);
	this->addChild(pQuitLevelFailed, 10);
}

bool LevelMapLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if(m_bIsSwipe)
	{
		return false;
	}
	m_bIsSwipe = true;

	if(m_pFooterNode->getSettingNode() != NULL && m_pFooterNode->getSettingNode()->getShowSetting())
	{
		return false;
	}

	Point touchPosition = pTouch->getLocation();
	m_fBeginY = touchPosition.y;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);

	return true;
}

void LevelMapLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{	 
	if (m_pButtonManagerNode->IsInTapMode())
		return;

	Point touchPosition = pTouch->getLocation();
	m_fYMoved = touchPosition.y - m_fBeginY;

	if (m_fYMoved + m_pBackgroundNode->getPosition().y >= 0) {
		m_fYMoved = -m_pBackgroundNode->getPosition().y;
	}
	else if(m_fYMoved + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-890))
	{
		m_fYMoved = -(m_maxHeight-890) - m_pBackgroundNode->getPosition().y;
	}
	
	Point point = m_pBackgroundNode->getPosition();
	m_pBackgroundNode->setPositionY(point.y + m_fYMoved);
	m_fBeginY = touchPosition.y;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);
}

void LevelMapLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if (m_pButtonManagerNode->IsInTapMode())
		return;

	DataTouch dataTouch = m_pScrollManager->getDistanceScrollY();
	float distanceY = dataTouch.point.y;
	float deltaTime = dataTouch.fDeltaTime;
	
	if(distanceY!=0 && deltaTime!=0)
	{
		float fTime = 0.2f;
		distanceY = distanceY * fTime / deltaTime / 10; 

		if (distanceY + m_pBackgroundNode->getPosition().y >= 0) {
			distanceY = -m_pBackgroundNode->getPosition().y;
		}
		else if(distanceY + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-890))
		{
			distanceY = -(m_maxHeight-890) - m_pBackgroundNode->getPosition().y;
		}

		auto actionMove = MoveBy::create(fTime, Point(0.0f, distanceY));
		auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
		m_pBackgroundNode->stopAllActions();
		m_pBackgroundNode->runAction(Sequence::create(actionEaseOut, NULL));
	}

	m_bIsSwipe = false;
}

Node* LevelMapLayer::generateLayoutStarAndBonusQuest(const int& iStarCompleted, const int& iBonusQuestCompleted, const int& iTotalBonusQuest)
{
	Node* pNode = Node::create();
	
	for(int iCountStar=0; iCountStar<3; iCountStar++)
	{
		Sprite* pStarSprite;
		if(iCountStar < iStarCompleted)
		{
			pStarSprite = Sprite::create("World-Map/star_win_small.png");
		}
		else
		{
			pStarSprite = Sprite::create("World-Map/star_target_small.png");
		}

		if (iCountStar == 0)
		{
			pStarSprite->setPosition(Point(0.0f, 0.0f));
			pStarSprite->setZOrder(1);
			pStarSprite->setScale(0.6f);
		}
		else if (iCountStar == 1)
		{
			pStarSprite->setPosition(Point(-30.0f, 9.0f));
			pStarSprite->setScale(0.5f);
		}
		else if (iCountStar == 2)
		{
			pStarSprite->setPosition(Point(30.0f, 9.0f));
			pStarSprite->setScale(0.5f);
		}

		pNode->addChild(pStarSprite);
	}

	for(int iCountBonusQuest=0; iCountBonusQuest<iTotalBonusQuest; iCountBonusQuest++)
	{
		Sprite* pBonusQuestSprite;
		if(iCountBonusQuest<iBonusQuestCompleted)
		{
			pBonusQuestSprite = Sprite::create("World-Map/mushroom_win.png");
		}
		else
		{
			pBonusQuestSprite = Sprite::create("World-Map/mushroom_fail.png");
		}
		pBonusQuestSprite->setScale(0.5f);
		pBonusQuestSprite->setPosition(Point(-15.0f*(iTotalBonusQuest-1) + iCountBonusQuest*27, 90.0f));

		pNode->addChild(pBonusQuestSprite);
	}

	return pNode;
}

void LevelMapLayer::updateLayoutWordMapWhenRetryLevelWin(const int& iCurrentLevel, const std::string sChapterId)
{
	m_levels = LevelTable::getInstance()->getAllLevelsForChapter(m_sChapterId);

	// Update layout pass level
	int iPassLevel = m_pButtonNodeCurrentLevel->getTag();
	Point pointPassLevel = m_pButtonNodeCurrentLevel->getPosition();

	if (iCurrentLevel >= iPassLevel && sChapterId == m_sChapterId)
	{
		m_pButtonManagerNode->removeButtonNode(m_pButtonNodeCurrentLevel);

		LevelInfo levelInfoPass = m_levels[iPassLevel-1];
		Node* pStarAndBonusQuestPassLevelNode = this->generateLayoutStarAndBonusQuest(levelInfoPass.iStar, levelInfoPass.iBonusQuest, levelInfoPass.iTotalBonusQuest);
		pStarAndBonusQuestPassLevelNode->setPosition(Point(pointPassLevel.x, pointPassLevel.y - 45));
		m_pBackgroundNode->addChild(pStarAndBonusQuestPassLevelNode);

		char sPassLevel[10];
		sprintf(sPassLevel, "%d", levelInfoPass.iLevel + m_iCalculatorLevel);
		LabelBMFont *pPassLevelLabel = LabelBMFont::create(sPassLevel, "fonts/font_title-popup.fnt");
		pPassLevelLabel->setScale(0.75f);
		pPassLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
		pPassLevelLabel->setPosition(Point(74.0f, 60.0f));

		Sprite* pButtonPassLevelSprite = Sprite::create("World-Map/pass-level.png");
		pButtonPassLevelSprite->addChild(pPassLevelLabel);

		m_buttonPlayPassLevel = ButtonNode::createButtonSprite(pButtonPassLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		m_buttonPlayPassLevel->setPosition(pointPassLevel);
		m_buttonPlayPassLevel->setTag(levelInfoPass.iLevel);
		m_pButtonManagerNode->addButtonNode(m_buttonPlayPassLevel);

		if (m_ButtonNextLevelSprite != NULL)
		{
			// Update layout current level
			m_iCurrentLevel = m_ButtonNextLevelSprite->getTag();
			Point pointCurrentLevel = m_ButtonNextLevelSprite->getPosition();

			m_pBackgroundNode->removeChild(m_ButtonNextLevelSprite);

			LevelInfo levelInfoCurrentLevel = m_levels[m_iCurrentLevel-1];
			Node* pStarAndBonusQuestCurrentLevelNode = this->generateLayoutStarAndBonusQuest(levelInfoCurrentLevel.iStar, levelInfoCurrentLevel.iBonusQuest, levelInfoCurrentLevel.iTotalBonusQuest);
			pStarAndBonusQuestCurrentLevelNode->setPosition(Point(pointCurrentLevel.x, pointCurrentLevel.y - 45));
			m_pBackgroundNode->addChild(pStarAndBonusQuestCurrentLevelNode);

			char sCurrentLevel[10];
			sprintf(sCurrentLevel, "%d", levelInfoCurrentLevel.iLevel + m_iCalculatorLevel);
			LabelBMFont *pCurrentLevelLabel = LabelBMFont::create(sCurrentLevel, "fonts/font_title-popup.fnt");
			pCurrentLevelLabel->setScale(0.75f);
			pCurrentLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
			pCurrentLevelLabel->setPosition(Point(74.0f, 60.0f));

			Sprite* pButtonCurrentLevelSprite = Sprite::create("World-Map/new-level.png");
			pButtonCurrentLevelSprite->addChild(pCurrentLevelLabel);

			ButtonNode* buttonPlayCurrentLevel = ButtonNode::createButtonSprite(pButtonCurrentLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
			buttonPlayCurrentLevel->setPosition(pointCurrentLevel);
			buttonPlayCurrentLevel->setTag(levelInfoCurrentLevel.iLevel);
			m_pButtonManagerNode->addButtonNode(buttonPlayCurrentLevel);
		}
	}
}

void LevelMapLayer::playEffectUnlockLevel(const bool& bPlayNextLevelGame, const int& iCurrentLevel, const std::string sChapterId)
{
	m_levels = LevelTable::getInstance()->getAllLevelsForChapter(m_sChapterId);

	// Update layout pass level
	int iPassLevel = m_pButtonNodeCurrentLevel->getTag();
	Point pointPassLevel = m_pButtonNodeCurrentLevel->getPosition();

	// Van Dao
	if(UserDefault::getInstance()->getIntegerForKey("IsUnlockALlLevel", 0) == 0)
	{
		if (iCurrentLevel >= iPassLevel && sChapterId == m_sChapterId)
		{
			LevelInfo levelInfoPass = m_levels[iPassLevel-1];
			m_pNodeStarAndBonusQuestEffect = Node::create();
			m_pNodeStarAndBonusQuestEffect->setPosition(Point(pointPassLevel.x, pointPassLevel.y - 45));
			m_pBackgroundNode->addChild(m_pNodeStarAndBonusQuestEffect);

			char sPassLevel[10];
			sprintf(sPassLevel, "%d", levelInfoPass.iLevel + m_iCalculatorLevel);
			LabelBMFont *pPassLevelLabel = LabelBMFont::create(sPassLevel, "fonts/font_title-popup.fnt");
			pPassLevelLabel->setScale(0.75f);
			pPassLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
			pPassLevelLabel->setPosition(Point(74.0f, 60.0f));

			Sprite* pButtonPassLevelSprite = Sprite::create("World-Map/pass-level.png");
			pButtonPassLevelSprite->addChild(pPassLevelLabel);

			m_buttonPlayPassLevel = ButtonNode::createButtonSprite(pButtonPassLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
			m_buttonPlayPassLevel->setPosition(pointPassLevel);
			m_buttonPlayPassLevel->setTag(levelInfoPass.iLevel);
			m_pButtonManagerNode->addButtonNode(m_buttonPlayPassLevel);

			m_iYellowStar = levelInfoPass.iStar;
			m_iCountYellowStar = 0;
			m_iTotalBonusQuest = levelInfoPass.iTotalBonusQuest;
			m_iBonusQuestCompleted = levelInfoPass.iBonusQuest;
			m_iCountBonusQuest = 0;

			this->sequenceUpdateStar();

			auto actionScaleInButtonCurrent = ScaleTo::create(0.3f, 0.5);
			auto actioneremoveButtonNodeCurrentLevel = CallFunc::create(this, callfunc_selector(LevelMapLayer::removeButtonNodeCurrentLevel));
			m_pButtonNodeCurrentLevel->runAction(Sequence::create(actionScaleInButtonCurrent, DelayTime::create(0.3f), actioneremoveButtonNodeCurrentLevel, NULL));

			m_buttonPlayPassLevel->setScale(0.4);
			auto actionScaleOut = ScaleTo::create(0.3f, 1.1f);
			auto actionScaleIn = ScaleTo::create(0.1f, 1.0f);
			auto actioneffectChangeCurrentLevel = CallFunc::create(this, callfunc_selector(LevelMapLayer::effectChangeCurrentLevel));
			auto actionshowPopupTargetGame = CallFunc::create(this, callfunc_selector(LevelMapLayer::showPopupTargetGame));

			if (bPlayNextLevelGame)
			{
				m_iCurrentLevel = iCurrentLevel;
				m_buttonPlayPassLevel->runAction(Sequence::create(DelayTime::create(0.3f), actionScaleOut, DelayTime::create(0.3f), actionScaleIn, DelayTime::create(0.3f), 
					actioneffectChangeCurrentLevel, DelayTime::create(0.6f), actionshowPopupTargetGame, NULL));
		
			}
			else
			{
				m_buttonPlayPassLevel->runAction(Sequence::create(DelayTime::create(0.3f), actionScaleOut, DelayTime::create(0.3f), actionScaleIn, DelayTime::create(0.3f), 
					actioneffectChangeCurrentLevel, NULL));
			}
		}
		else
		{
			if (bPlayNextLevelGame)
			{
				m_iCurrentLevel = iCurrentLevel;
				this->showPopupTargetGame();
			}
		}
	}
	else
	{
		// Van Dao
		updateLayoutWordMapWhenRetryLevelWin(iCurrentLevel, sChapterId);
		if (bPlayNextLevelGame)
		{
			m_iCurrentLevel = iCurrentLevel;
			this->showPopupTargetGame();
		}
	}
}

void LevelMapLayer::sequenceUpdateStar()
{
	auto actionUpdateStar = CallFunc::create(this, callfunc_selector(LevelMapLayer::updateStar));
	auto delay = DelayTime::create(0.3f);
	if (m_iCountYellowStar < m_iYellowStar) {
		this->runAction(Sequence::create(delay->clone(), actionUpdateStar, NULL));
	}
	else
	{
		this->sequenceUpdateBonusQuest();
	}
}

void LevelMapLayer::updateStar()
{
	if(m_iCountYellowStar < m_iYellowStar)
	{
		Sprite* pStarSprite = Sprite::create("World-Map/star_win_small.png");
		m_pNodeStarAndBonusQuestEffect->addChild(pStarSprite);

		if (m_iCountYellowStar == 0)
		{
			pStarSprite->setPosition(Point(0.0f, 0.0f));
			pStarSprite->setZOrder(1);
			pStarSprite->setScale(0.6f);
		}
		else if (m_iCountYellowStar == 1)
		{
			pStarSprite->setPosition(Point(-30.0f, 9.0f));
			pStarSprite->setScale(0.5f);
		}
		else if (m_iCountYellowStar == 2)
		{
			pStarSprite->setPosition(Point(30.0f, 9.0f));
			pStarSprite->setScale(0.5f);
		}

		m_iCountYellowStar++;
		this->sequenceUpdateStar();
	}
}

void LevelMapLayer::sequenceUpdateBonusQuest()
{
	auto actionUpdateBonusQuest = CallFunc::create(this, callfunc_selector(LevelMapLayer::updateBonusQuest));
	auto delay = DelayTime::create(0.3f);
	this->runAction(Sequence::create(delay->clone(), actionUpdateBonusQuest, NULL));
}

void LevelMapLayer::updateBonusQuest()
{
	if (m_iCountBonusQuest < m_iBonusQuestCompleted && m_iCountBonusQuest < m_iTotalBonusQuest)
	{
		Sprite* pBonusQuestSprite = Sprite::create("World-Map/mushroom_win.png");
		pBonusQuestSprite->setScale(0.5f);
		pBonusQuestSprite->setPosition(Point(-15.0f + m_iCountBonusQuest*27, 90.0f));
		m_pNodeStarAndBonusQuestEffect->addChild(pBonusQuestSprite);

		m_iCountBonusQuest++;
		this->sequenceUpdateBonusQuest();
	}
}

void LevelMapLayer::effectChangeCurrentLevel()
{
	if (m_ButtonNextLevelSprite != NULL)
	{
		// Update layout current level
		m_iCurrentLevel = m_ButtonNextLevelSprite->getTag();
		Point pointCurrentLevel = m_ButtonNextLevelSprite->getPosition();

		m_pBackgroundNode->removeChild(m_ButtonNextLevelSprite);

		LevelInfo levelInfoCurrentLevel = m_levels[m_iCurrentLevel-1];
		Node* pStarAndBonusQuestCurrentLevelNode = this->generateLayoutStarAndBonusQuest(levelInfoCurrentLevel.iStar, levelInfoCurrentLevel.iBonusQuest, levelInfoCurrentLevel.iTotalBonusQuest);
		pStarAndBonusQuestCurrentLevelNode->setPosition(Point(pointCurrentLevel.x, pointCurrentLevel.y - 45));
		m_pBackgroundNode->addChild(pStarAndBonusQuestCurrentLevelNode);

		pStarAndBonusQuestCurrentLevelNode->setScale(0.4f);
		auto actionScaleOutStar = ScaleTo::create(0.3f, 1.1f);
		auto actionScaleInStar = ScaleTo::create(0.1f, 1.0f);
		pStarAndBonusQuestCurrentLevelNode->runAction(Sequence::create(actionScaleOutStar, DelayTime::create(0.3f), actionScaleInStar, NULL));

		char sCurrentLevel[10];
		sprintf(sCurrentLevel, "%d", levelInfoCurrentLevel.iLevel + m_iCalculatorLevel);
		LabelBMFont *pCurrentLevelLabel = LabelBMFont::create(sCurrentLevel, "fonts/font_title-popup.fnt");
		pCurrentLevelLabel->setScale(0.75f);
		pCurrentLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
		pCurrentLevelLabel->setPosition(Point(74.0f, 60.0f));

		Sprite* pButtonCurrentLevelSprite = Sprite::create("World-Map/new-level.png");
		pButtonCurrentLevelSprite->addChild(pCurrentLevelLabel);

		ButtonNode* buttonPlayCurrentLevel = ButtonNode::createButtonSprite(pButtonCurrentLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		buttonPlayCurrentLevel->setPosition(pointCurrentLevel);
		buttonPlayCurrentLevel->setTag(levelInfoCurrentLevel.iLevel);
		m_pButtonManagerNode->addButtonNode(buttonPlayCurrentLevel);

		buttonPlayCurrentLevel->setScale(0.4f);
		auto actionScaleOut = ScaleTo::create(0.3f, 1.1f);
		auto actionScaleIn = ScaleTo::create(0.1f, 1.0f);
		buttonPlayCurrentLevel->runAction(Sequence::create(actionScaleOut, DelayTime::create(0.3f), actionScaleIn, NULL));
	}
}

void LevelMapLayer::showPopupTargetGame()
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(m_sChapterId, m_iCurrentLevel);

	GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), m_iCurrentLevel, m_sChapterId);
	this->getParent()->addChild(pGameTargetNode, 10);
}

void LevelMapLayer::removeButtonNodeCurrentLevel()
{
	m_pButtonManagerNode->removeButtonNode(m_pButtonNodeCurrentLevel);
}