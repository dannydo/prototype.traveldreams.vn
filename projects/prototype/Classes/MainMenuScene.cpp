#include "MainMenuScene.h"
#include "HelloWorldScene.h"
#include "cocos2d.h"
#include "GameTargetNode.h"
#include "SoundManager.h"
#include "IntroductionLayer.h"

using namespace cocos2d;

bool MainMenuScene::init()
{
	if(CCScene::init())
	{
		this->_layer = MainMenuLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

MainMenuScene::~MainMenuScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

bool MainMenuLayer::init()
{
	if (CCLayerColor::initWithColor(ccc4(10,20,30,255)))
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile( "Menu.plist", "Menu.pkm");
		
		
		do{
			//create menu
			CCMenuItemImage* pLevel1Item = CCMenuItemImage::create(
				"MenuItem01.png",
				"MenuItem01.png",
				CC_CALLBACK_1(MainMenuLayer::menuItemSelected, this));
				//this,
				//menu_selector(MainMenuLayer::menuItemSelected));
			CC_BREAK_IF(!pLevel1Item);
			pLevel1Item->setTag(1);
			pLevel1Item->setPosition(ccp(winSize.width /2.f, winSize.height/2.f - 90.f));

			CCMenuItemImage* pLevel2Item = CCMenuItemImage::create(
				"MenuItem02.png",
				"MenuItem02.png",
				CC_CALLBACK_1(MainMenuLayer::menuItemSelected, this));
				//this,
				//menu_selector(MainMenuLayer::menuItemSelected));
			CC_BREAK_IF(!pLevel2Item);
			pLevel2Item->setTag(2);
			pLevel2Item->setPosition(ccp(winSize.width /2.f, winSize.height/2.f - 30.f));

			CCMenuItemImage* pLevel3Item = CCMenuItemImage::create(
				"MenuItem03.png",
				"MenuItem03.png",
				CC_CALLBACK_1(MainMenuLayer::menuItemSelected, this));
				//this,
				//menu_selector(MainMenuLayer::menuItemSelected));
			CC_BREAK_IF(!pLevel3Item);
			pLevel3Item->setTag(3);
			pLevel3Item->setPosition(ccp(winSize.width /2.f, winSize.height/2.f + 30.f));	

			CCMenuItemImage* pLevel4Item = CCMenuItemImage::create(
				"MenuItem04.png",
				"MenuItem04.png",
				CC_CALLBACK_1(MainMenuLayer::menuItemSelected, this));
				//this,
				//menu_selector(MainMenuLayer::menuItemSelected));
			CC_BREAK_IF(!pLevel4Item);
			pLevel4Item->setTag(4);
			pLevel4Item->setPosition(ccp(winSize.width /2.f, winSize.height/2.f + 90.f));	
			

			CCMenu* pMenu = CCMenu::create( pLevel1Item, pLevel2Item, pLevel3Item,pLevel4Item, NULL);
			pMenu->setPosition(CCPointZero);
			CC_BREAK_IF(!pMenu);

			this->addChild(pMenu, 1);


			/*CCSize winSizeX = CCDirector::sharedDirector()->getWinSize();
			CCLabelTTF* _label = CCLabelTTF::create(" wq ewqe qweqwew qwe", "Arial", 32);
			_label->retain();
			_label->setColor( ccc3(0,0,0));
			_label->setPosition(ccp(winSizeX.width/2, winSizeX.height/2));
			this->addChild(_label, 1);*/

			SoundManager::PlayIntroMusic();

		}while(0);

		//create menu
		CCMenuItemImage* pIntroduction = CCMenuItemImage::create(
			"Introduction/button_introduction.png",
			"Introduction/button_introduction.png",
			CC_CALLBACK_0(MainMenuLayer::menuOpenIntroduction, this));
		pIntroduction->setPosition(ccp(35, 35));

		CCMenu* pMenuIntroduction = CCMenu::create(pIntroduction , NULL);
		pMenuIntroduction->setPosition(CCPointZero);
		this->addChild(pMenuIntroduction);
		
		return true;
	}
	else
	{
		return false;
	}
}

void MainMenuLayer::menuItemSelected(CCObject* pSender)
{
	CCMenuItem* pMenuItem = (CCMenuItem*)pSender;
	int iLevel = pMenuItem->getTag();

	GameWordManager* pGameWordManager = GameWordManager::getInstance();
	pGameWordManager->GenerateWordForNewLevel(iLevel);

	GameTargetNode* pGameTargetNode = GameTargetNode::createLayout(pGameWordManager->GetMainWord(), iLevel);
	this->addChild(pGameTargetNode, 10);
	/*
	CCScene *pGameScene = HelloWorld::createScene(iLevel); //CCScene::create();        
    //HelloWorld *pGameLayer = HelloWorld::create();
	//pGameLayer->initLevel(iLevel);
	//pGameScene->addChild(pGameLayer);
	CCDirector::getInstance()->replaceScene(pGameScene);
	*/
}

void MainMenuLayer::menuOpenIntroduction()
{
	IntroductionScene* pIntroduction = IntroductionScene::create();
	Director::getInstance()->replaceScene(pIntroduction);
}

MainMenuLayer::~MainMenuLayer()
{

}