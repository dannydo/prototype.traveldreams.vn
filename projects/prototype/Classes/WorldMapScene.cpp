#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "IntroductionLayer.h"
#include "FlashCardCollection.h"
#include "MainMenuScene.h"
#include "SoundManager.h"
#include "Database\ChapterTable.h"
#include "ButtonManagerNode.h"
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
	m_pBackgroundNode->setPosition(Point(0.0f, 94.0f));
	this->addChild(m_pBackgroundNode);

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	m_pBackgroundNode->addChild(pButtonManagerNode);

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
				pButtonManagerNode->addButtonNode(buttonPlayNode);
			}
			else
			{
				Sprite* pButtonLevelSprite = Sprite::create(sPathFile.c_str());
				pButtonLevelSprite->setPosition(wordMapChapterConfig.m_position);
				m_pBackgroundNode->addChild(pButtonLevelSprite);
			} 
		}
		else
		{
			Sprite* pButtonLevelSprite = Sprite::create(sPathFile.c_str());
			pButtonLevelSprite->setPosition(wordMapChapterConfig.m_position);
			m_pBackgroundNode->addChild(pButtonLevelSprite);
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
	m_bMoveSlideShow = false;

	return true;
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