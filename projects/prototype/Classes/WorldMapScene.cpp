#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "IntroductionLayer.h"
#include "FlashCardCollection.h"
#include "MainMenuScene.h"
#include "SoundManager.h"
#include "Database\ChapterTable.h"
#include "HeaderNode.h"

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

	m_wordMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
	std::string sPathFile = "GameData/";
	sPathFile.append(m_wordMapConfig.m_sFileNameBackgound);

	Sprite* pBackgroundSprite=  Sprite::create(sPathFile.c_str());
	pBackgroundSprite->setAnchorPoint(Point(0,0));
	this->addChild(pBackgroundSprite);

	m_pBackgroundNode = LayerColor::create(ccc4(255, 255, 255, 0));
	m_pBackgroundNode->setContentSize(CCSizeMake(640.0f, 960.0f));
	m_pBackgroundNode->setAnchorPoint(Point(0.5f, 0.0f));
	m_pBackgroundNode->setPosition(Point(0.0f, 60.0f));
	this->addChild(m_pBackgroundNode);

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->AllowSwipingBackground(true);
	m_pBackgroundNode->addChild(m_pButtonManagerNode);

	UserInfo userInfo = UserTable::getInstance()->getUserInfo();
	std::vector<ChapterInfo> chapters = ChapterTable::getInstance()->getChaptersInfo();

	m_maxHeight=960.0f;

	WordlMapConfig::WordMapChapterConfig wordMapChapterConfig;
	for(int iIndex=0; iIndex<m_wordMapConfig.m_WorlMapChapterConfigs.size(); iIndex++)
	{
		wordMapChapterConfig = m_wordMapConfig.m_WorlMapChapterConfigs[iIndex];
		sPathFile = wordMapChapterConfig.m_sPathData;
		sPathFile.append("/Chapter-icon.png");

		if (iIndex < chapters.size())
		{
			ChapterInfo chapterInfo = chapters[iIndex];
			if(chapterInfo.bIsUnlock)
			{
				Sprite* pButtonLevelSprite = Sprite::create(sPathFile.c_str());
				pButtonLevelSprite->setTag(iIndex);
				ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonLevelSprite, CC_CALLBACK_1(WorldMapLayer::menuPlayChapterCallBack, this));
				buttonPlayNode->setPosition(wordMapChapterConfig.m_position);
				buttonPlayNode->setTag(iIndex);
				m_pButtonManagerNode->addButtonNode(buttonPlayNode);
			}
			else
			{
				Sprite* pButtonLevelSprite = Sprite::create(sPathFile.c_str());
				pButtonLevelSprite->setColor(ccc3(46, 48, 107));
				pButtonLevelSprite->setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_A8_COLOR));
				pButtonLevelSprite->setPosition(wordMapChapterConfig.m_position);
				m_pBackgroundNode->addChild(pButtonLevelSprite);

				Node* pNodeLockChapter = this->createLayoutLockChapter();
				pNodeLockChapter->setPosition(wordMapChapterConfig.m_position);
				m_pBackgroundNode->addChild(pNodeLockChapter);
			} 
		}
		else
		{
			Sprite* pButtonLevelSprite = Sprite::create(sPathFile.c_str());
			pButtonLevelSprite->setColor(ccc3(46, 48, 107));
			pButtonLevelSprite->setShaderProgram(ShaderCache::getInstance()->getProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_A8_COLOR));
			pButtonLevelSprite->setPosition(wordMapChapterConfig.m_position);
			m_pBackgroundNode->addChild(pButtonLevelSprite);

			Node* pNodeLockChapter = this->createLayoutLockChapter();
			pNodeLockChapter->setPosition(wordMapChapterConfig.m_position);
			m_pBackgroundNode->addChild(pNodeLockChapter);
		}

		if (wordMapChapterConfig.m_position.y > m_maxHeight)
		{
			m_maxHeight = wordMapChapterConfig.m_position.y + 250;
		}
	}

	m_pFooterNode = FooterNode::create();
	this->addChild(m_pFooterNode);

	HeaderNode* pHeaderNode = HeaderNode::create();
	this->addChild(pHeaderNode);

	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kWorldMap);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	m_pScrollManager = new ScrollManager();
	m_bIsSwipe = false;

	return true;
}

cocos2d::Node* WorldMapLayer::createLayoutLockChapter()
{
	Node* pNodeLock = Node::create();

	Sprite* pBGRight = Sprite::create("World-Map/cloud_rig.png");
	pBGRight->setPosition(Point(50.0f, 0.0f));
	pNodeLock->addChild(pBGRight);

	Sprite* pBGLeft = Sprite::create("World-Map/cloud_lef.png");
	pBGLeft->setPosition(Point(20.0f, 10.0f));
	pNodeLock->addChild(pBGLeft);

	Sprite* pBGLock = Sprite::create("World-Map/chapter_lock.png");
	pNodeLock->addChild(pBGLock);

	return pNodeLock;
}

void WorldMapLayer::menuPlayChapterCallBack(Object* sender)
{
	// play sound effect
	SoundManager::PlaySoundEffect(_SET_CHAPTER_SELECT_);

	ButtonNode* pButtonNode = (ButtonNode*)sender;
	int iIndex = pButtonNode->getTag();
	std::string sChapterId = m_wordMapConfig.m_WorlMapChapterConfigs[iIndex].m_sChapterId;

	UserDefault::getInstance()->setStringForKey("ChapterPlayGame", sChapterId);
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}

bool WorldMapLayer::onTouchBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
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

void WorldMapLayer::onTouchMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	if (m_pButtonManagerNode->IsInTapMode())
		return;

	Point touchPosition = pTouch->getLocation();
	m_fYMoved = touchPosition.y - m_fBeginY;

	if (m_fYMoved + m_pBackgroundNode->getPosition().y >= 60.0f) {
		m_fYMoved = -m_pBackgroundNode->getPosition().y + 60.0f;
	}
	else if(m_fYMoved + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-887))
	{
		m_fYMoved = -(m_maxHeight-887) - m_pBackgroundNode->getPosition().y;
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

void WorldMapLayer::onTouchEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
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

		if (distanceY + m_pBackgroundNode->getPosition().y >= 60.0f) {
			distanceY = -m_pBackgroundNode->getPosition().y + 60.0f;
		}
		else if(distanceY + m_pBackgroundNode->getPosition().y <= -(m_maxHeight-887))
		{
			distanceY = -(m_maxHeight-887) - m_pBackgroundNode->getPosition().y;
		}

		auto actionMove = MoveBy::create(fTime, Point(0.0f, distanceY));
		auto actionEaseOut = EaseOut::create(actionMove, 2.5f);
		m_pBackgroundNode->stopAllActions();
		m_pBackgroundNode->runAction(Sequence::create(actionEaseOut, NULL));
	}

	m_bIsSwipe = false;
}