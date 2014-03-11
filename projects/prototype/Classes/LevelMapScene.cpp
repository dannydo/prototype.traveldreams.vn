#include "LevelMapScene.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "GameTargetNode.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "FlashCardCollection.h"
#include "Database\UserTable.h"
#include "LifeSystemNode.h"
#include "Database\LevelTable.h"
#include "ButtonManagerNode.h"
#include "Database\UserTable.h"

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
	m_pBackgroundNode->setPosition(Point(0.0f, 94.0f));
	this->addChild(m_pBackgroundNode);
	m_maxHeight=0;;

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

	while(!levels.empty())
	{
		LevelInfo levelInfo = levels.back();
		Point point = m_pointLevel.back();

		char sLevel[5];
		sprintf(sLevel, "%d", levelInfo.iLevel);
		LabelTTF* pLevelLabel = LabelTTF::create(sLevel, "Arial", 32);
		pLevelLabel->setAnchorPoint(Point(0.5f, 0.5f));
		pLevelLabel->setPosition(point);
		
		if(levelInfo.bIsUnlock || levelInfo.iLevel == userInfo.iCurrentLevel)
		{
			Sprite* pButtonLevelSprite = Sprite::create("World-Map/Level_lock.png");
			pButtonLevelSprite->setTag(levelInfo.iLevel);
			ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonLevelSprite, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
			buttonPlayNode->setPosition(point);
			buttonPlayNode->setTag(levelInfo.iLevel);
			pButtonManagerNode->addButtonNode(buttonPlayNode);

			if(levelInfo.iLevel == userInfo.iCurrentLevel)
			{
				pLevelLabel->setColor(ccc3(0, 255, 0));
			}
		}
		else
		{
			Sprite* pButtonLevelSprite = Sprite::create("World-Map/Level_lock.png");
			pButtonLevelSprite->setPosition(point);
			m_pBackgroundNode->addChild(pButtonLevelSprite);
			pLevelLabel->setColor(ccc3(255, 0, 0));
		}

		m_pBackgroundNode->addChild(pLevelLabel);

		levels.pop_back();
		m_pointLevel.pop_back();
	}

	Sprite* pBarBottom = Sprite::create("FlashCard/bar-bottom.jpg");
	pBarBottom->setAnchorPoint(Point(0.5f, 0.0f));
	pBarBottom->setPosition(Point(winSize.width/2.0f, 0));
	this->addChild(pBarBottom);

	CCMenuItemImage* pItemSettingMenu = CCMenuItemImage::create(
		"World-Map/mask-button-back.png",
		"World-Map/mask-button-back.png",
		CC_CALLBACK_0(LevelMapLayer::menuBackToWorldMap, this));
	pItemSettingMenu->setPosition(ccp(56, 49));

	CCMenu* pMenu = CCMenu::create(pItemSettingMenu, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu);
	
	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);

	LifeSystemNode* pLifeNode = LifeSystemNode::create();
	pLifeNode->setPosition(Point(50.0f, 910.0f));
	this->addChild(pLifeNode);

	m_pSettingNode = NULL;
	m_isShowSetting = false;
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kLevelMap);
	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	m_bMoveSlideShow = false;

	return true;
}

void LevelMapLayer::menuLevelSelected(CCObject* pSender)
{
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{
		ButtonNode* pButtonNode = (ButtonNode*)pSender;
		int iLevel = pButtonNode->getTag();

		GameWordManager* pGameWordManager = GameWordManager::getInstance();
		pGameWordManager->GenerateWordForNewLevel(iLevel);

		CCLOG("Init -3: level %d", iLevel); 

		GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), iLevel);
		this->addChild(pGameTargetNode, 10);

		CCLOG("Init -4"); 
	}
	else
	{
		MessageBox("You have no life!", "Play Level");
	}
}

void LevelMapLayer::menuBackToWorldMap()
{
	if(m_pSettingNode == NULL)
	{
		m_pSettingNode = SettingMenuNode::create();
		m_pSettingNode->setPosition(Point(-500.0f, 0));
		this->addChild(m_pSettingNode);
	}

	if (m_isShowSetting == false)
	{
		m_isShowSetting = true;
		m_pSettingNode->show();
		this->setTouchEnabled(false);
	}
	else
	{
		m_isShowSetting = false;
		m_pSettingNode->hide();
		this->setTouchEnabled(true);
	}
}

void LevelMapLayer::menuOpenFlashCardCallBack()
{
	FlashCardCollectionScene* pFlashCardCollection = FlashCardCollectionScene::create();
	pFlashCardCollection->getLayerColor()->setNameClassParent("LevelMap");
	Director::getInstance()->replaceScene(pFlashCardCollection);
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
	Point touchPosition = pTouch->getLocation();
	m_fBeginY = touchPosition.y;

	return true;
}

void LevelMapLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{	 
	if(m_bMoveSlideShow == false)
	{
		Point touchPosition = pTouch->getLocation();
		m_fYMoved = touchPosition.y - m_fBeginY;

		if (m_fYMoved + m_pBackgroundNode->getPosition().y >= 94) {
			m_fYMoved = -m_pBackgroundNode->getPosition().y + 94;
		}
		else if(m_fYMoved + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-960))
		{
			m_fYMoved = -(m_maxHeight-960) - m_pBackgroundNode->getPosition().y;
		}
	
		m_bMoveSlideShow = true;
		auto actionMove = MoveBy::create(0.0f, Point(0.0f, m_fYMoved));
		auto actionUpdate = CallFunc::create(this, callfunc_selector(LevelMapLayer::updateScrollSlideShow));
		m_pBackgroundNode->runAction(Sequence::create(actionMove, actionUpdate, NULL));
		m_fBeginY = touchPosition.y;
	}
}

void LevelMapLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{

}

void LevelMapLayer::updateScrollSlideShow()
{
	m_bMoveSlideShow = false;
}