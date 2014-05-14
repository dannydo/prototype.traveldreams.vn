#include "MainMenuScene.h"
#include "SoundManager.h"
#include "WorldMapScene.h"
#include "ButtonNode.h"
#include "StatusLayer.h"
#include "SystemEventHandle.h"
#include "AdvanceModeMyPackagesScene.h"

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
	Sprite* pSettingSpriteActive = Sprite::create("Footer/btn_setting.png");
	m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, pSettingSpriteActive, CC_CALLBACK_1(MainMenuLayer::openSettingMenu, this));
	m_pButtonSettingNode->setPosition(Point(36.0f, 33.0f));
	
	//test time mode
	Sprite* pButtonTestTimeModeGameSprite = Sprite::create("LoadingAndMainMenu/btn_custom.png");
	ButtonNode* buttonTestTimeModeNode = ButtonNode::createButtonSprite(pButtonTestTimeModeGameSprite, CC_CALLBACK_1(MainMenuLayer::startTimeModeDemo, this));
	buttonTestTimeModeNode->setPosition(Point(320.0f, 407.0f));

	/*
	LabelBMFont* pEnviteFriendsLabel = LabelBMFont::create("Invite Friends", "fonts/font_score.fnt");
	pEnviteFriendsLabel->setAnchorPoint(Point(0.5f, 0.5f));
	pEnviteFriendsLabel->setPosition(Point(230.0f, 52.0f));

	Sprite* pButtonInviteSprite = Sprite::create("PanelSetting/btn_unlock_all_level.png");
	pButtonInviteSprite->addChild(pEnviteFriendsLabel);
	ButtonNode* m_buttonInvite = ButtonNode::createButtonSprite(pButtonInviteSprite, CC_CALLBACK_1(MainMenuLayer::clickInviteFriends, this));
	m_buttonInvite->setPosition(Point(320.0f, 190.0f));
	*/

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->addButtonNode(buttonPlayNode);	
	m_pButtonManagerNode->addButtonNode(m_pButtonSettingNode);
	m_pButtonManagerNode->addButtonNode(buttonTestTimeModeNode);
	//m_pButtonManagerNode->addButtonNode(m_buttonInvite);
	this->addChild(m_pButtonManagerNode);


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
	Sprite* pButtonLoginFacebookSprite = Sprite::create("LoadingAndMainMenu/btn_login.png");
	m_buttonLoginNode = ButtonNode::createButtonSprite(pButtonLoginFacebookSprite, CC_CALLBACK_1(MainMenuLayer::loginFacebook, this));
	m_buttonLoginNode->setPosition(Point(320.0f, 297.0f));
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
	else if(!FacebookManager::getInstance()->isLogined() && m_isAddButtonLogin == true &&  UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", -1) == 0)
	{
		this->addButtonLoginFacebook();
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

void MainMenuLayer::clickInviteFriends(Object* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	FacebookManager::getInstance()->inviteFriends("Invite friends play game", "Invite Friends", "['app_non_users']");
#else
	MessageBox("Facebook not run with platform window", "Facebook");
#endif
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
		m_pSettingNode->setPosition(Point(-505.0f, 0.0f));
		m_pSettingNode->addButtonSetting(m_pButtonSettingNode);
		this->addChild(m_pSettingNode);
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(FacebookManager::getInstance()->isLogined() && UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) == 1)
	{
		m_pSettingNode->setStatusButtonFacebook(0);
		CCLOG("logout");
	}
	else if(!FacebookManager::getInstance()->isLogined() &&  UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", -1) == 0)
	{
		m_pSettingNode->setStatusButtonFacebook(1);
		CCLOG("login");
	}
#endif

	if (m_pSettingNode->getShowSetting() == false)
	{
		m_pSettingNode->show();
	}
	else
	{
		m_pSettingNode->hide();
	}
}


#include "HelloWorldScene.h"
#include "GameWordManager.h"
#include "CustomPackageDownloaderNode.h"

// test time mode
void MainMenuLayer::startTimeModeDemo(cocos2d::Object* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if(!FacebookManager::getInstance()->isLogined() || UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) != 1)
	{
		MessageBox("Please login facebook before play advance mode game!","");
		return;
	}
#endif

	/*
	auto scene = AdvanceModeMyPackagesScene::create();
	Director::getInstance()->replaceScene(scene);
	*/

	CustomPackageDownloaderNode* pNode = CustomPackageDownloaderNode::create();
	pNode->setPosition(Point(40.f, 200.f));
	this->addChild(pNode, 1000);
}