#include "LevelMapScene.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "GameTargetNode.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "FlashCardScene.h"

using namespace cocos2d;

bool LevelMapScene::init()
{
	if(CCScene::init())
	{
		this->_layer =LevelMapLayer::create();
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
	if (CCLayer::init())
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		Sprite* background = Sprite::create("World-Map/LEVEL.jpg");
		background->setPosition(Point(winSize.width/2.0f, winSize.height/2.0f));
		this->addChild(background);

		//create level item
		CCMenuItemImage* pLevel1Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel1Item->setTag(1);
		pLevel1Item->setPosition(ccp(115, 185));

		CCMenuItemImage* pLevel2Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel2Item->setTag(2);
		pLevel2Item->setPosition(ccp(370, 190));

		CCMenuItemImage* pLevel3Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel3Item->setTag(3);
		pLevel3Item->setPosition(ccp(543, 300));	

		CCMenuItemImage* pLevel4Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel4Item->setTag(4);
		pLevel4Item->setPosition(ccp(265, 395));

		CCMenuItemImage* pLevel5Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel5Item->setTag(2);
		pLevel5Item->setPosition(ccp(96, 555));

		CCMenuItemImage* pLevel6Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel6Item->setTag(3);
		pLevel6Item->setPosition(ccp(526, 813));

		CCMenuItemImage* pBackItem = CCMenuItemImage::create(
			"World-Map/mask-button-back.png",
			"World-Map/mask-button-back.png",
			CC_CALLBACK_0(LevelMapLayer::menuBackToWorldMap, this));
		pBackItem->setPosition(ccp(56, 49));

		CCMenuItemImage* pFlashCard = CCMenuItemImage::create(
		"World-Map/mask-button-flash-card.png",
		"World-Map/mask-button-flash-card.png",
		CC_CALLBACK_0(LevelMapLayer::menuOpenFlashCardCallBack, this));
		pFlashCard->setPosition(ccp(365, 52));
			
		CCMenu* pMenu = CCMenu::create( pLevel1Item, pLevel2Item, pLevel3Item, pLevel4Item, pBackItem, pFlashCard, NULL);
		pMenu->setPosition(CCPointZero);
			
		this->addChild(pMenu, 1);

		SoundManager::PlayIntroMusic();

		return true;
	}
	else
	{
		return false;
	}
}

void LevelMapLayer::menuLevelSelected(CCObject* pSender)
{
	CCMenuItem* pMenuItem = (CCMenuItem*)pSender;
	int iLevel = pMenuItem->getTag();

	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(iLevel);

	GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(const_cast<char*>(pGameWordManager->GetMainWord().m_sWord), iLevel);
	this->addChild(pGameTargetNode, 10);
}

void LevelMapLayer::menuBackToWorldMap()
{
	WorldMapScene* pWorldMap = WorldMapScene::create();
	Director::getInstance()->replaceScene(pWorldMap);
}

void LevelMapLayer::menuOpenFlashCardCallBack()
{
	FlashCardScene* pFlashCard = FlashCardScene::create();
	pFlashCard->getLayer()->setNameClassParent("WorldMapScene");
	Director::getInstance()->replaceScene(pFlashCard);
}