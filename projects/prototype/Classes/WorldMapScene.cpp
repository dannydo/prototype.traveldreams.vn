#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "IntroductionLayer.h"
#include "FlashCardCollection.h"
#include "MainMenuScene.h"
#include "LifeSystemNode.h"
#include "SoundManager.h"
#include "GameBoardManager.h"
#include "Database\ChapterTable.h"
#include "ButtonManagerNode.h"

USING_NS_CC;

bool WorldMapScene::init()
{
	if(CCScene::init())
	{
		this->_layer = WorldMapLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}

	return false;
}

WorldMapScene::~WorldMapScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

WorldMapLayer::~WorldMapLayer()
{

}

bool WorldMapLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	this->loadConfigWordMap();

	Sprite* pBackgroundSprite=  Sprite::create("World-Map/Worldmap_BG.png");
	pBackgroundSprite->setAnchorPoint(Point(0,0));
	this->addChild(pBackgroundSprite);

	m_pBackgroundNode = LayerColor::create(ccc4(255, 255, 255, 0));
	m_pBackgroundNode->setContentSize(CCSizeMake(640.0f, 960.0f));
	m_pBackgroundNode->setAnchorPoint(Point(0.5f, 0.0f));
	m_pBackgroundNode->setPosition(Point(0.0f, 94.0f));
	this->addChild(m_pBackgroundNode);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	m_pBackgroundNode->addChild(pButtonManagerNode);

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	std::vector<ChapterInfo> chapters = ChapterTable::getInstance()->getChaptersInfo();

	m_maxHeight=960.0f;

	while(!chapters.empty())
	{
		ChapterInfo chapterInfo = chapters.back();
		Point point = m_pointChapter.back();

		char sFilename[50];
		sprintf(sFilename, "World-Map/%d.png", chapterInfo.iChapter); 

		if(chapterInfo.bIsUnlock || chapterInfo.iChapter == userInfo.iCurrentChapter || 1 == 1)
		{
			Sprite* pButtonLevelSprite = Sprite::create(sFilename);
			pButtonLevelSprite->setTag(chapterInfo.iChapter);
			ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonLevelSprite, CC_CALLBACK_1(WorldMapLayer::menuPlayChapterCallBack, this));
			buttonPlayNode->setPosition(point);
			buttonPlayNode->setTag(chapterInfo.iChapter);
			pButtonManagerNode->addButtonNode(buttonPlayNode);

			if(chapterInfo.iChapter == userInfo.iCurrentLevel)
			{
				
			}
		}
		else
		{
			Sprite* pButtonLevelSprite = Sprite::create(sFilename);
			pButtonLevelSprite->setPosition(point);
			m_pBackgroundNode->addChild(pButtonLevelSprite);
		}

		chapters.pop_back();
		m_pointChapter.pop_back();
	}

	m_pFooterNode = FooterNode::create();
	this->addChild(m_pFooterNode);

	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kWorldMap);

	
	// temporary add background header to node
	Sprite* pBarTop = Sprite::create("World-Map/Header.png");
	pBarTop->setAnchorPoint(Point(0.0f, 1.f));	
	pBarTop->setPosition( Point(0, winSize.height));
	this->addChild(pBarTop);

	LifeSystemNode* pLifeNode = LifeSystemNode::create();
	pLifeNode->setPosition(Point(50.0f, 910.0f));
	this->addChild(pLifeNode);



	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	m_bMoveSlideShow = false;

	return true;
}

void WorldMapLayer::menuPlayChapterCallBack(Object* sender)
{
	// play sound effect 					
	SoundManager::PlaySoundEffect(_SET_CHAPTER_SELECT_);

	ButtonNode* pButtonNode = (ButtonNode*)sender;
	int iChapter = pButtonNode->getTag();

	UserDefault::getInstance()->setIntegerForKey("ChapterPlayGame", iChapter);
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}

void WorldMapLayer::loadConfigWordMap()
{
	unsigned long iDataSize;
	unsigned char* orginalData = cocos2d::CCFileUtils::getInstance()->getFileData("World-Map/config.txt", "r", &iDataSize);
	char* data = new char[iDataSize];
	memcpy(data, orginalData, iDataSize);
	membuf dataBuf(data, data + iDataSize);
	std::istream inputStream(&dataBuf);

	inputStream >> m_iTotalChapter;

	std::string sTemp;
	for(int iIndex = 0; iIndex < m_iTotalChapter; iIndex++)
	{
		float fX, fY;
		inputStream >> fX;
		inputStream >> fY;
		Point point = Point(fX, fY);
		m_pointChapter.push_back(point);
	}

	delete[] data;
	delete[] orginalData;
}

bool WorldMapLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if(m_pFooterNode->getSettingNode() != NULL && m_pFooterNode->getSettingNode()->getShowSetting())
	{
		return false;
	}

	Point touchPosition = pTouch->getLocation();
	m_fBeginY = touchPosition.y;

	return true;
}

void WorldMapLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
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
		auto actionUpdate = CallFunc::create(this, callfunc_selector(WorldMapLayer::updateScrollSlideShow));
		m_pBackgroundNode->runAction(Sequence::create(actionMove, actionUpdate, NULL));
		m_fBeginY = touchPosition.y;
	}
}

void WorldMapLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{

}

void WorldMapLayer::updateScrollSlideShow()
{
	m_bMoveSlideShow = false;
}