#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "Database\InitDatabase.h"
#include "Database\UserTable.h"
#include "SettingMenuNode.h"
#include "Database\DictionaryDatabase.h"	 
#include "GameConfigManager.h"
#include "GameWordManager.h"
#include "Database\VersionTable.h"
#include "APIService\SyncDataGame.h"

using namespace cocos2d;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

bool LoadingScene::init()
{
	if(CCScene::init())
	{
		this->_layer = LoadingLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

LoadingScene::~LoadingScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

LoadingLayer::~LoadingLayer()
{
	
}

bool LoadingLayer::init()
{
	if (!LayerColor::initWithColor(ccc4(255, 255, 255, 255)))
	{
		return false;
	}

	Sprite* pBackgroundSprite = Sprite::create("LoadingAndMainMenu/loading-screen.png");
	pBackgroundSprite->setAnchorPoint(Point(0.5f, 0.5f));
	pBackgroundSprite->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackgroundSprite);

	Sprite* pLoadingSprite = Sprite::create("LoadingAndMainMenu/loading-text.png");
	pLoadingSprite->setAnchorPoint(Point(0.5f, 0.5f));
	pLoadingSprite->setPosition(Point(320.0f, 660.0f));
	this->addChild(pLoadingSprite);

	m_bFinishLoad = false;
	this->scheduleUpdate();
	this->loadData();
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kExitGame);

	return true;
}

void LoadingLayer::update(float dt)
{
	if (m_bFinishLoad && SyncDataGame::getInstance()->getIsFinishSync())
	{
		MainMenuScene* scene = MainMenuScene::create();
		Director::getInstance()->replaceScene(scene);
	}
}

void LoadingLayer::loadData()
{
	auto actionLoading = CallFunc::create(this, callfunc_selector(LoadingLayer::initData));
	auto actionFinishLoading = CallFunc::create(this, callfunc_selector(LoadingLayer::finishLoading));

	this->runAction(Sequence::create(actionLoading, actionFinishLoading, NULL));
}

void LoadingLayer::initData()
{
	// load word list
	GameWordManager::getInstance()->LoadWorldListConfig();
	GameWordManager::getInstance()->LoadWordGenerateConfig();

	// load level config
	GameConfigManager::getInstance()->LoadGameConfig();
	GameConfigManager::getInstance()->LoadObstacleConfig();
	GameConfigManager::getInstance()->LoadWordMapConfig();

	GameConfigManager::getInstance()->LoadCustomModeConfig();

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnMusic", -1) == -1)
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnMusic", 1);
	}

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnEffect", -1) == -1)
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnEffect", 1);
	}

	if(UserDefault::getInstance()->getIntegerForKey("SettingTurnOnVoice", -1) == -1)
	{
		UserDefault::getInstance()->setIntegerForKey("SettingTurnOnVoice", 1);
	}

	UserDefault::getInstance()->setIntegerForKey("NumberConnectServer", 0);

	InitDatabase::getInstance();
	DictionaryDatabase::getInstance();

	if(InitDatabase::getInstance()->getDatabseSqlite() != NULL)
	{
		if (UserDefault::getInstance()->getIntegerForKey("InitDatabase", 0) == 0)
		{
			WordlMapConfig worldMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
			WordlMapConfig::WordMapChapterConfig worldMapChapterConfig = worldMapConfig.m_WorlMapChapterConfigs[0];
	
			UserInfo userInfo =  UserTable::getInstance()->getUserInfo();
			userInfo.sCurrentChapterId = worldMapChapterConfig.m_sChapterId;
			userInfo.iCurrentLevel = 1;
			UserTable::getInstance()->updateUser(userInfo);

			// Create data for one chapter
			std::vector<std::string> wordList;
			std::vector<int> mapLevels;
			GameConfigManager::getInstance()->GenerateWordsForLevels(worldMapChapterConfig.m_sChapterId, wordList, mapLevels);

			if(InitDatabase::getInstance()->createDataChapterAndLevel(worldMapChapterConfig.m_sChapterId, wordList, mapLevels))
			{
				UserDefault::getInstance()->setIntegerForKey("InitDatabase", 1);
			}
		}
	}

	if(UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", -1) == -1)
	{
		UserDefault::getInstance()->setIntegerForKey("IsLoginFacebook", 0);
	}

	if(UserDefault::getInstance()->getIntegerForKey("IsLoginFacebook", 0) == 1)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			FacebookManager::getInstance()->loadPlugin();
			FacebookManager::getInstance()->autoOpenActiveSession();
		#endif
	}

	// Sync Data Game
	SyncDataGame::getInstance()->runSyncDataGame();
}

void LoadingLayer::finishLoading()
{
	m_bFinishLoad = true;
}