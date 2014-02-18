#include "MainMenuScene.h"
#include "SoundManager.h"
#include "WorldMapScene.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "FacebookManager.h"
using namespace cocos2d::plugin;
#endif

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
	pPlayGame->setPosition(Point(winSize.width /2.f, winSize.height/2.f - 105));

	CCMenuItemImage* m_pLoginfacebook = CCMenuItemImage::create(
		"LoadingAndMainMenu/facebook_login_button.png",
		"LoadingAndMainMenu/facebook_login_button.png",
		CC_CALLBACK_0(MainMenuLayer::loginFacebook, this));
	m_pLoginfacebook->setPosition(Point(winSize.width /2.f, winSize.height/2.f - 250));

	CCMenu* pMenu = CCMenu::create( pPlayGame, m_pLoginfacebook,  NULL);
	pMenu->setPosition(CCPointZero);

	this->addChild(pMenu, 1);
	SoundManager::PlayIntroMusic();

    LabelTTF* label2 = LabelTTF::create("Logout", "Arial", 32);
    MenuItemLabel* m_pItemLogout = MenuItemLabel::create(label2, CC_CALLBACK_0(MainMenuLayer::LogoutFacebook, this));
    m_pItemLogout->setPosition(Point(winSize.width /2.f - 40, winSize.height/2.f - 350));
	pMenu->addChild(m_pItemLogout);

	LabelTTF* label3 = LabelTTF::create("Share", "Arial", 32);
    MenuItemLabel* m_pItemShareLink = MenuItemLabel::create(label3, CC_CALLBACK_0(MainMenuLayer::shareLinkFacebook, this));
    m_pItemShareLink->setPosition(Point(winSize.width /2.f - 190, winSize.height/2.f - 350));
	pMenu->addChild(m_pItemShareLink);

	LabelTTF* label4 = LabelTTF::create("Share Dialog", "Arial", 32);
    MenuItemLabel* m_pItemShareDialog = MenuItemLabel::create(label4, CC_CALLBACK_0(MainMenuLayer::shareDialogFacebook, this));
    m_pItemShareDialog->setPosition(Point(winSize.width /2.f + 150, winSize.height/2.f - 350));
	pMenu->addChild(m_pItemShareDialog);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
#endif

	return true;
}

void MainMenuLayer::playGame()
{
	WorldMapScene* scene = WorldMapScene::create();
	Director::getInstance()->replaceScene(scene);
}

void MainMenuLayer::loginFacebook()
{
	CCLOG("login");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    FacebookManager::getInstance()->loginByMode();
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}

void MainMenuLayer::LogoutFacebook()
{
	CCLOG("logout");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    FacebookManager::getInstance()->logoutByMode();
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}

void MainMenuLayer::shareLinkFacebook()
{
	CCLOG("Share facebook link");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    FacebookManager::getInstance()->shareLink("Facebook SDK for Android", "Build great social apps and get more installs.",
		"The Facebook SDK for Android makes it easier and faster to develop Facebook integrated Android apps.",
		"https://developers.facebook.com/android",
		"https://raw.github.com/fbsamples/ios-3.x-howtos/master/Images/iossdk_logo.png");
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
	
}

void MainMenuLayer::shareDialogFacebook()
{
	CCLOG("Share facebook dialog");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    FacebookManager::getInstance()->shareDialog("Facebook SDK for Android", "Build great social apps and get more installs.",
		"The Facebook SDK for Android makes it easier and faster to develop Facebook integrated Android apps.",
		"https://developers.facebook.com/android",
		"https://raw.github.com/fbsamples/ios-3.x-howtos/master/Images/iossdk_logo.png");
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}
