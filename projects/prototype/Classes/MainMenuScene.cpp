#include "MainMenuScene.h"
#include "SoundManager.h"
#include "WorldMapScene.h"

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

MainMenuLayer::~MainMenuLayer()
{

}

bool MainMenuLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	Sprite* pSpriteBackground = Sprite::create("LoadingAndMainMenu/background.png");
	pSpriteBackground->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteBackground->setPosition(Point(winSize.width/2.0f, winSize.height/2.0f));
	this->addChild(pSpriteBackground);

	Sprite* pSpriteTitle = Sprite::create("LoadingAndMainMenu/title.png");
	pSpriteTitle->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteTitle->setPosition(Point(winSize.width/2.0f, winSize.height - 215));
	this->addChild(pSpriteTitle);
	
	CCMenuItemImage* pPlayGame = CCMenuItemImage::create(
		"LoadingAndMainMenu/Play-button.png",
		"LoadingAndMainMenu/Play-button.png",
		CC_CALLBACK_0(MainMenuLayer::playGame, this));
	pPlayGame->setPosition(ccp(winSize.width /2.f, winSize.height/2.f - 105));

	CCMenuItemImage* pLoginfacebook = CCMenuItemImage::create(
		"LoadingAndMainMenu/facebook_login_button.png",
		"LoadingAndMainMenu/facebook_login_button.png",
		CC_CALLBACK_0(MainMenuLayer::loginFacebook, this));
	pLoginfacebook->setPosition(ccp(winSize.width /2.f, winSize.height/2.f - 275.f));

	CCMenu* pMenu = CCMenu::create( pPlayGame, pLoginfacebook,  NULL);
	pMenu->setPosition(CCPointZero);

	this->addChild(pMenu, 1);
	SoundManager::PlayIntroMusic();

	return true;
}

void MainMenuLayer::playGame()
{
	WorldMapScene* scene = WorldMapScene::create();
	Director::getInstance()->replaceScene(scene);
}

void MainMenuLayer::loginFacebook()
{

}