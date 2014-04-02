#include "AppDelegate.h"
#include "GameWordManager.h"
#include "GameConfigManager.h"
#include "SimpleAudioEngine.h"
#include "LoadingScene.h"

#include "APIService\UserService.h"
#include "Social\FacebookManager.h"

#include "Database\InitDatabase.h"
#include "Database\ChapterTable.h"
#include "Database\FriendTable.h"
#include "Database\LevelTable.h"
#include "Database\UserTable.h"
#include "Database\DictionaryDatabase.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
	GameWordManager::getInstance()->releaseInstance();
	GameConfigManager::releaseInstance();

	UserService::releaseInstance();

	ChapterTable::releaseInstance();
	FriendTable::releaseInstance();
	LevelTable::releaseInstance();
	UserTable::releaseInstance();
	InitDatabase::releaseInstance();
	DictionaryDatabase::releaseInstance();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//FacebookManager::getInstance()->unloadPlugin();
#endif
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto eglView = EGLView::getInstance();	

    director->setOpenGLView(eglView);
	
	eglView->setDesignResolutionSize(640, 960.f, ResolutionPolicy::SHOW_ALL);

    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	// load word list
	GameWordManager::getInstance()->LoadWorldListConfig();
	GameWordManager::getInstance()->LoadWordGenerateConfig();

	// load level config
	GameConfigManager::getInstance()->LoadGameConfig();
	GameConfigManager::getInstance()->LoadObstacleConfig();
	GameConfigManager::getInstance()->LoadLevelsConfig();


    // create a scene. it's an autorelease object
	auto scene =  LoadingScene::create(); //MainMenuScene::create(); //HelloWorld::createScene();		

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
     //SimpleAudioEngine::()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
     //SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
