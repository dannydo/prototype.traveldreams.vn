#include "MainMenuScene.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "ButtonNode.h"
#include "StatusLayer.h"
#include "SystemEventHandle.h"

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
}

bool MainMenuLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	SoundManager::PlayBackgroundMusic(SoundManager::StateBackGroundMusic::kIntroMusic);
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	Sprite* pSpriteBackground = Sprite::create("LoadingAndMainMenu/background.png");
	pSpriteBackground->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteBackground->setPosition(Point(winSize.width/2.0f, winSize.height/2.0f));
	this->addChild(pSpriteBackground);

	Sprite* pButtonPlayGameSprite = Sprite::create("LoadingAndMainMenu/btn_play.png");
	ButtonNode* buttonPlayNode = ButtonNode::createButtonSprite(pButtonPlayGameSprite, CC_CALLBACK_1(MainMenuLayer::playGame, this));
	buttonPlayNode->setPosition(Point(320.0f, 515.0f));

	Sprite* pSettingSprite = Sprite::create("Footer/btn_setting.png");
	Sprite* pSettingSpriteActive = Sprite::create("Footer/btn-back-menu.png");
	m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, pSettingSpriteActive, CC_CALLBACK_1(MainMenuLayer::openSettingMenu, this));
	m_pButtonSettingNode->setPosition(Point(50.0f, 50.0f));
	
	//test time mode
	Sprite* pButtonTestTimeModeGameSprite = Sprite::create("LoadingAndMainMenu/btn_play.png");
	//pButtonTestTimeModeGameSprite->setScaleX(1.5f);
	//pButtonTestTimeModeGameSprite->setScaleY(0.75f);
	pButtonTestTimeModeGameSprite->setColor(Color3B( 100, 200, 200));
	ButtonNode* buttonTestTimeModeNode = ButtonNode::createButtonSprite(pButtonTestTimeModeGameSprite, CC_CALLBACK_1(MainMenuLayer::startTimeModeDemo, this));
	buttonTestTimeModeNode->setPosition(Point(320.0f, 215.0f));

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->addButtonNode(buttonPlayNode);	
	m_pButtonManagerNode->addButtonNode(m_pButtonSettingNode);

	m_pButtonManagerNode->addButtonNode(buttonTestTimeModeNode);
	this->addChild(m_pButtonManagerNode, 10);

	this->addButtonLoginFacebook();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->loadPlugin();
	if(FacebookManager::getInstance()->isLogined())
	{
		m_pButtonManagerNode->removeButtonNode(m_buttonLoginNode);
		m_isAddButtonLogin = true;
	}
#endif

	m_pSettingNode = NULL;
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kMainMenu);
	this->scheduleUpdate();

	return true;
}

void MainMenuLayer::addButtonLoginFacebook()
{
	Sprite* pButtonLoginFacebookSprite = Sprite::create("LoadingAndMainMenu/FB_btn.png");
	m_buttonLoginNode = ButtonNode::createButtonSprite(pButtonLoginFacebookSprite, CC_CALLBACK_1(MainMenuLayer::loginFacebook, this));
	m_buttonLoginNode->setPosition(Point(320.0f, 355.0f));
	m_pButtonManagerNode->addButtonNode(m_buttonLoginNode);
	m_isAddButtonLogin = false;
}

void MainMenuLayer::update(float dt)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(FacebookManager::getInstance()->isLogined() && m_isAddButtonLogin == false && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) == 1)
	{
		m_sFacebookToken = FacebookManager::getInstance()->getAccessToken();
		m_pButtonManagerNode->removeButtonNode(m_buttonLoginNode);
		m_isAddButtonLogin = true;
	}
#endif
}

void MainMenuLayer::playGame(Object* sender)
{
	WorldMapScene* scene = WorldMapScene::create();
	Director::getInstance()->replaceScene(scene);
}

void MainMenuLayer::loginFacebook(Object* sender)
{
	UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
	if (m_pSettingNode != NULL)
		m_pSettingNode->setStatusButtonFacebook(1);
	SystemEventHandle::getInstance()->onStartConnectFacebook();
}

/*
void MainMenuLayer::shareLinkFacebook()
{
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
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    FacebookManager::getInstance()->shareDialog("Facebook SDK for Android", "Build great social apps and get more installs.",
		"The Facebook SDK for Android makes it easier and faster to develop Facebook integrated Android apps.",
		"https://developers.facebook.com/android",
		"https://raw.github.com/fbsamples/ios-3.x-howtos/master/Images/iossdk_logo.png");
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
}
*/

void MainMenuLayer::openSettingMenu(Object *sender)
{
	if(m_pSettingNode == NULL)
	{
		m_pSettingNode = SettingMenuNode::create();
		m_pSettingNode->setPosition(Point(-524.0f, 0.0f));
		m_pSettingNode->addButtonSetting(m_pButtonSettingNode);
		this->addChild(m_pSettingNode);
	}

	if (m_pSettingNode->getShowSetting() == false)
	{
		m_pSettingNode->show();
	}
	else
	{
		m_pSettingNode->hide();
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if(!FacebookManager::getInstance()->isLogined() && m_isAddButtonLogin == true)
		{
			this->addButtonLoginFacebook();
		}
		#endif
	}
}


#include "HelloWorldScene.h"
#include "GameWordManager.h"
#include "CustomPackageDownloaderNode.h"

// test time mode
void MainMenuLayer::startTimeModeDemo(cocos2d::Object* sender)
{
	CustomPackageDownloaderNode* pNode = CustomPackageDownloaderNode::create();
	pNode->setPosition(Point(40.f, 200.f));
	this->addChild(pNode, 1000);

	/*
	auto timeModeConfig = GameConfigManager::getInstance()->GetTimeModeDemoConfig();	
	GameWordManager::getInstance()->GenerateWordForNewLevelOfTimeMode(&timeModeConfig);

	//WorldMapScene* scene = WorldMapScene::create();
	//Director::getInstance()->replaceScene(scene);

	auto scene = HelloWorld::createScene( _GMT_TIME_MODE_);
	Director::getInstance()->replaceScene(scene);*/
}