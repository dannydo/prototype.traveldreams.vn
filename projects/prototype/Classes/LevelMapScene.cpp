#include "LevelMapScene.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "GameTargetNode.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "FlashCardCollection.h"
#include "Database\UserTable.h"
#include "HeaderNode.h"
#include "Database\LevelTable.h"
#include "ButtonManagerNode.h"
#include "Database\UserTable.h"	 
#include "Config.h"

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
	
}

bool LevelMapLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

	int iChapter = UserDefault::getInstance()->getIntegerForKey("ChapterPlayGame", 1);
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	this->loadConfigChapter(iChapter);

	m_pBackgroundNode = Node::create();
	m_pBackgroundNode->setAnchorPoint(Point(0.5f, 0.0f));
	m_pBackgroundNode->setPosition(Point(0.0f, 0.0f));
	this->addChild(m_pBackgroundNode);
	m_maxHeight=0;

	for(int iIndex=1; iIndex<=m_iTotalImageBG; iIndex++)
	{
		char sBGChapter[50];
		sprintf(sBGChapter, "World-Map/%d/bg%d.png", iChapter, iIndex);
		Sprite* pBGImage = Sprite::create(sBGChapter);
		pBGImage->setAnchorPoint(Point(0.5f, 0.0f));
		pBGImage->setPosition(Point(320.0f, m_maxHeight));

		m_pBackgroundNode->addChild(pBGImage);
		m_maxHeight += pBGImage->getContentSize().height;
	}

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	m_pBackgroundNode->addChild(pButtonManagerNode);

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	std::vector<LevelInfo> levels = LevelTable::getInstance()->fetchLevelsForChapter(iChapter);

	Point pointScroll;

	while(!levels.empty())
	{
		LevelInfo levelInfo = levels.back();
		Point point = m_pointLevel.back();

		char sLevel[5];
		sprintf(sLevel, "%d", levelInfo.iLevel);
		LabelTTF* pLevelLabel = LabelTTF::create(sLevel, "fonts/UTM Cookies.ttf", 32);
		pLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
		pLevelLabel->setPosition(Point(point.x-2, point.y+17));
		
		if(levelInfo.bIsUnlock || levelInfo.iLevel == userInfo.iCurrentLevel || levelInfo.iLevel < 31)
		{
			Sprite* pButtonLevelSprite;
			
			
			if(levelInfo.iLevel == userInfo.iCurrentLevel)
			{
				pointScroll = point;
				pButtonLevelSprite = Sprite::create("World-Map/new-level.png");
			}
			else
			{
				pButtonLevelSprite = Sprite::create("World-Map/pass-level.png");
			}

			pButtonLevelSprite->setTag(levelInfo.iLevel);
			ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
			buttonPlayNode->setPosition(point);
			buttonPlayNode->setTag(levelInfo.iLevel);
			pButtonManagerNode->addButtonNode(buttonPlayNode);

			if (levelInfo.bIsUnlock || levelInfo.iLevel == userInfo.iCurrentLevel)
			{
				Node* pStarAndBonusQuestNode = this->generateLayoutStarAndBonusQuest(levelInfo.iStar, levelInfo.iBonusQuest, levelInfo.iTotalBonusQuest);
				pStarAndBonusQuestNode->setPosition(Point(point.x-2, point.y));
				m_pBackgroundNode->addChild(pStarAndBonusQuestNode);
			}
		}
		else
		{
			Sprite* pButtonLevelSprite = Sprite::create("World-Map/Level_lock.png");
			pButtonLevelSprite->setPosition(point);
			m_pBackgroundNode->addChild(pButtonLevelSprite);
		}

		m_pBackgroundNode->addChild(pLevelLabel);

		levels.pop_back();
		m_pointLevel.pop_back();
	}

	if (pointScroll.y > 960-94-200 && pointScroll.y < m_maxHeight-960) {
		m_pBackgroundNode->setPositionY(-pointScroll.y + 450);
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
	m_bMoveSlideShow = false;

	m_pScrollManager = new ScrollManager();

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

		GameWordManager* pGameWordManager = GameWordManager::getInstance();
		pGameWordManager->GenerateWordForNewLevel(iLevel);		

		GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), iLevel);
		this->addChild(pGameTargetNode, 10);		
	}
	else
	{
		MessageBox("You have no life!", "Play Level");
	}
}

void LevelMapLayer::showPopupTargetGame(const int& iLevel)
{
	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(iLevel);

	GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), iLevel);
	this->addChild(pGameTargetNode, 10);
}

void LevelMapLayer::loadConfigChapter(const int& iChapter)
{
	char sFilename[50];
	sprintf(sFilename, "World-Map/%d/config.txt", iChapter);

	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData(sFilename, "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> m_iTotalImageBG;
	inputStream >> m_iTotalLevel;

	std::string sTemp;
	for(int iIndex = 0; iIndex < m_iTotalLevel; iIndex++)
	{
		float fX, fY;
		inputStream >> fX;
		inputStream >> fY;
		Point point = Point(fX, fY);
		m_pointLevel.push_back(point);
	}

	delete[] data;
	delete[] orginalData;
}

bool LevelMapLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
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
	Point touchPosition = pTouch->getLocation();
	m_fYMoved = touchPosition.y - m_fBeginY;

	if (m_fYMoved + m_pBackgroundNode->getPosition().y >= 0) {
		m_fYMoved = -m_pBackgroundNode->getPosition().y;
	}
	else if(m_fYMoved + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-960))
	{
		m_fYMoved = -(m_maxHeight-960) - m_pBackgroundNode->getPosition().y;
	}
	
	Point point = m_pBackgroundNode->getPosition();
	m_pBackgroundNode->setPosition(Point(point.x, point.y + m_fYMoved));
	m_fBeginY = touchPosition.y;

	DataTouch dataTouch;
	dataTouch.point = touchPosition;
	dataTouch.lTime = 0;
	dataTouch.fDeltaTime = 0;
	m_pScrollManager->addDataToQueue(dataTouch);
}

void LevelMapLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{

	DataTouch dataTouch = m_pScrollManager->getDistanceScrollY();
	float distanceY = dataTouch.point.y;
	float deltaTime = dataTouch.fDeltaTime;
	

	float fTime = 0.2f;
	distanceY = distanceY * fTime / deltaTime / 10; 

	if (distanceY + m_pBackgroundNode->getPosition().y >= 0) {
		distanceY = -m_pBackgroundNode->getPosition().y;
	}
	else if(distanceY + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-960))
	{
		distanceY = -(m_maxHeight-960) - m_pBackgroundNode->getPosition().y;
	}

	auto actionMove = MoveBy::create(fTime, Point(0.0f, distanceY));
	auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
	m_pBackgroundNode->stopAllActions();
	m_pBackgroundNode->runAction(Sequence::create(actionEaseOut, NULL));
}

void LevelMapLayer::updateScrollSlideShow()
{
	m_bMoveSlideShow = false;
}

Node* LevelMapLayer::generateLayoutStarAndBonusQuest(const int& iStarCompleted, const int& iBonusQuestCompleted, const int& iTotalBonusQuest)
{
	Node* pNode = Node::create();
	int fRadiusCircle = 60;
	Point point;

	for(int iCountStar=0; iCountStar<3; iCountStar++)
	{
		Sprite* pStarSprite;
		if(iCountStar < iStarCompleted)
		{
			pStarSprite = Sprite::create("World-Map/start-y.png");
		}
		else
		{
			pStarSprite = Sprite::create("World-Map/star-p.png");
		}

		float angle = _C_PI_-(58 - 20*iTotalBonusQuest + 37*iCountStar)/180.0f*_C_PI_;
		point.x = fRadiusCircle*cos(angle);
		point.y = fRadiusCircle*sin(angle);

		pStarSprite->setPosition(point);
		pNode->addChild(pStarSprite);
	}

	for(int iCountBonusQuest=0; iCountBonusQuest<iTotalBonusQuest; iCountBonusQuest++)
	{
		Sprite* pBonusQuestSprite;
		if(iCountBonusQuest<iBonusQuestCompleted)
		{
			pBonusQuestSprite = Sprite::create("World-Map/bonus-y.png");
		}
		else
		{
			pBonusQuestSprite = Sprite::create("World-Map/bonus-p.png");
		}

		float angle = _C_PI_-(58 - 20*iTotalBonusQuest + 37*(iCountBonusQuest+3))/180.0f*_C_PI_;
		point.x = fRadiusCircle*cos(angle);
		point.y = fRadiusCircle*sin(angle);

		pBonusQuestSprite->setPosition(point);
		pNode->addChild(pBonusQuestSprite);
	}

	return pNode;
}