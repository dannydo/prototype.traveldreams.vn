#include "LevelMapScene.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "GameTargetNode.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "FlashCardCollection.h"
#include "Database\UserTable.h"
#include "LifeSystemNode.h"

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
		pLevel5Item->setTag(5);
		pLevel5Item->setPosition(ccp(96, 555));

		CCMenuItemImage* pLevel6Item = CCMenuItemImage::create(
			"World-Map/mask-level.png",
			"World-Map/mask-level.png",
			CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
		pLevel6Item->setTag(8);
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
			
		CCMenu* pMenu = CCMenu::create( pLevel1Item, pLevel2Item, pLevel3Item, pLevel4Item, pLevel5Item, pLevel6Item, pBackItem, pFlashCard, NULL);
		pMenu->setPosition(CCPointZero);
			
		this->addChild(pMenu, 1);
		SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);

		LifeSystemNode* pLifeNode = LifeSystemNode::create();
		pLifeNode->setPosition(Point(50.0f, 910.0f));
		this->addChild(pLifeNode);

		m_pSettingNode = NULL;
		m_isShowSetting = false;
		Breadcrumb::getInstance()->addSceneMode(SceneMode::kLevelMap);

		for (int iIndex=1; iIndex<=30; iIndex++)
		{
			char sLevel[5];
			sprintf(sLevel, "%d", iIndex);
			LabelTTF* pLabelLevel = LabelTTF::create(sLevel, "Arial", 40);
			MenuItemLabel* pLevelItem = MenuItemLabel::create(pLabelLevel, CC_CALLBACK_1(LevelMapLayer::menuLevelSelected, this));
			pLevelItem->setTag(iIndex);
			if (iIndex%10 == 0)
			{
				pLevelItem->setPosition(Point(((iIndex-1)%10 + 1)*59, 740-(int(iIndex/10)-1)*50));
			}
			else
			{
				pLevelItem->setPosition(Point(((iIndex-1)%10 + 1)*59, 740-(int(iIndex/10))*50));
			}
			pMenu->addChild(pLevelItem);
		}

		return true;
	}
	else
	{
		return false;
	}
}

void LevelMapLayer::menuLevelSelected(CCObject* pSender)
{
	if(UserTable::getInstance()->getUserInfo().iLife > 0)
	{
		CCMenuItem* pMenuItem = (CCMenuItem*)pSender;
		int iLevel = pMenuItem->getTag();

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