#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "IntroductionLayer.h"
#include "FlashCardCollection.h"
#include "MainMenuScene.h"
#include "LifeSystemNode.h"
#include "SoundManager.h"

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

	Sprite* background = Sprite::create("World-Map/world MAP.jpg");
	background->setPosition(Point(winSize.width/2.0f, winSize.height/2.0f));
	this->addChild(background);

	//create menu
	MenuItemImage* pChapterItem1 = MenuItemImage::create(
		"World-Map/mask.png",
		"World-Map/mask.png",
		CC_CALLBACK_0(WorldMapLayer::menuPlayChapterCallBack, this));
	pChapterItem1->setPosition(ccp(465, 248));

	MenuItemImage* pChapterItem2 = MenuItemImage::create(
		"World-Map/mask.png",
		"World-Map/mask.png",
		CC_CALLBACK_0(WorldMapLayer::menuPlayChapterCallBack, this));
	pChapterItem2->setPosition(ccp(175, 483));

	MenuItemImage* pChapterItem3 = MenuItemImage::create(
		"World-Map/mask.png",
		"World-Map/mask.png",
		CC_CALLBACK_0(WorldMapLayer::menuPlayChapterCallBack, this));
	pChapterItem3->setPosition(ccp(500, 758));

	CCMenuItemImage* pIntroduction = CCMenuItemImage::create(
		"World-Map/info_icon.png",
		"World-Map/info_icon.png",
		CC_CALLBACK_0(WorldMapLayer::menuOpenIntroductionCallBack, this));
	pIntroduction->setPosition(ccp(150, 48));
	pIntroduction->setScale(1.2f);

	CCMenuItemImage* pFlashCard = CCMenuItemImage::create(
		"World-Map/mask-button-flash-card.png",
		"World-Map/mask-button-flash-card.png",
		CC_CALLBACK_0(WorldMapLayer::menuOpenFlashCardCallBack, this));
	pFlashCard->setPosition(ccp(365, 52));

	CCMenuItemImage* pItemSetting = CCMenuItemImage::create(
		"World-Map/mask-button-back.png",
		"World-Map/mask-button-back.png",
		CC_CALLBACK_0(WorldMapLayer::openSettingMenu, this));
	pItemSetting->setPosition(ccp(56, 49));

	Menu* menuChapter = Menu::create(pChapterItem1 , pChapterItem2, pChapterItem3, pIntroduction, pFlashCard, pItemSetting, NULL);
	menuChapter->setPosition(CCPointZero);
	this->addChild(menuChapter);

	LifeSystemNode* pLifeNode = LifeSystemNode::create();
	pLifeNode->setPosition(Point(50.0f, 910.0f));
	this->addChild(pLifeNode);

	m_pSettingNode = NULL;
	m_isShowSetting = false;
	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kWorldMap);

	return true;
}

void WorldMapLayer::menuPlayChapterCallBack()
{
	UserDefault::getInstance()->setIntegerForKey("ChapterPlayGame", 1);
	LevelMapScene* pLevelMap =  LevelMapScene::create();
	Director::getInstance()->replaceScene(pLevelMap);
}

void WorldMapLayer::menuOpenIntroductionCallBack()
{
	IntroductionScene* pIntroduction = IntroductionScene::create();
	Director::getInstance()->replaceScene(pIntroduction);
}

void WorldMapLayer::menuOpenFlashCardCallBack()
{
	FlashCardCollectionScene* pFlashCardCollection = FlashCardCollectionScene::create();
	pFlashCardCollection->getLayerColor()->setNameClassParent("WorldMap");
	Director::getInstance()->replaceScene(pFlashCardCollection);
}

void WorldMapLayer::openSettingMenu()
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