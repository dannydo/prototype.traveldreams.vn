#include "MainMenuScene.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "LifeSystemNode.h"	
#include "DictionaryNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
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
	UserService::getInstance()->removeCallBackList(this);
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
	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);

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
	
	LifeSystemNode* pLifeNode = LifeSystemNode::create();
	pLifeNode->setPosition(Point(50.0f, 910.0f));
	this->addChild(pLifeNode);

	this->scheduleUpdate();

	// menu layer with close item	
	auto closeItem = MenuItemImage::create("Setting.png",
                                           "Setting.png",
                                           CC_CALLBACK_0(MainMenuLayer::openSettingMenu, this));
    
	closeItem->setAnchorPoint(ccp(0,0));
	closeItem->setPosition(ccp(-12, -8));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
	this->addChild(menu);

	m_pSettingNode = NULL;
	m_isShowSetting = false;
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kMainMenu);

	return true;
}

void MainMenuLayer::update(float dt)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(FacebookManager::getInstance()->isLogined())
	{
		UserService::getInstance()->addCallBackList(this);
		UserService::getInstance()->registryUser(FacebookManager::getInstance()->getAccessToken());
		
		this->unscheduleUpdate();
	}
#endif
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

void MainMenuLayer::openSettingMenu()
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

void MainMenuLayer::resultHttpRequestCompleted(cs::JsonDictionary* pJsonDict, std::string sKey)
{
	
}