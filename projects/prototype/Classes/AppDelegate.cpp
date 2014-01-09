#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "LevelMapScene.h"
#include "GameWordManager.h"
#include "WorldMapScene.h"
#include "GameConfigManager.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
	GameWordManager::getInstance()->releaseInstance();
	GameConfigManager::releaseInstance();
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
	GameWordManager::getInstance()->LoadWords();
	GameWordManager::getInstance()->LoadWordGenerateConfig();

	// load level config
	GameConfigManager::getInstance()->LoadGameConfig();
	GameConfigManager::getInstance()->LoadLevelsConfig();

    // create a scene. it's an autorelease object
	auto scene =  WorldMapScene::create(); //MainMenuScene::create(); //HelloWorld::createScene();		

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
