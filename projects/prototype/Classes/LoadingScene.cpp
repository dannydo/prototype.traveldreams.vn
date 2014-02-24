#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "Database\InitDatabase.h"
#include "Database\UserTable.h"

using namespace cocos2d;

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

	Size winSize = Director::getInstance()->getWinSize();

	Sprite* pSpriteLogo = Sprite::create("LoadingAndMainMenu/KISS-logo.png");
	pSpriteLogo->setAnchorPoint(Point(0.5f, 0.5f));
	pSpriteLogo->setPosition(Point(winSize.width/2.0f, winSize.height/2.0f));
	this->addChild(pSpriteLogo);

	Label* pLabelLoading = Label::createWithTTF("Loading...", "fonts/ARLRDBD.ttf", 30);
	pLabelLoading->setColor(ccc3(0, 0, 0));
	pLabelLoading->setAnchorPoint(Point(0.5f, 0.5f));
	pLabelLoading->setPosition(Point(winSize.width/2.0f, winSize.height/2.0f - pSpriteLogo->getContentSize().height));
	this->addChild(pLabelLoading);

	m_bFinishLoad = false;
	this->scheduleUpdate();
	this->loadData();

	return true;
}

void LoadingLayer::update(float dt)
{
	if (m_bFinishLoad)
	{
		MainMenuScene* scene = MainMenuScene::create();
		Director::getInstance()->replaceScene(scene);
	}
}

void LoadingLayer::loadData()
{
	auto actionLoading = CallFunc::create(this, callfunc_selector(LoadingLayer::initDatase));
	auto actionFinishLoading = CallFunc::create(this, callfunc_selector(LoadingLayer::finishLoading));

	this->runAction(Sequence::create(actionLoading, actionFinishLoading, NULL));
}

void LoadingLayer::initDatase()
{
	InitDatabase::getInstance();
	UserTable::getInstance();
}

void LoadingLayer::finishLoading()
{
	m_bFinishLoad = true;
}