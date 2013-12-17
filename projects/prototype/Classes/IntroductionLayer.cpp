#include "IntroductionLayer.h"
#include "MainMenuScene.h"

USING_NS_CC;

bool IntroductionScene::init()
{
	if(CCScene::init())
	{
		this->_layer = IntroductionLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}

	return false;
}

IntroductionScene::~IntroductionScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

IntroductionLayer::~IntroductionLayer()
{

}

bool IntroductionLayer::init()
{
	if (!CCLayerColor::initWithColor(ccc4(191, 103, 241, 229)))
	{
		return false;
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	m_pNodeImage = Node::create();
	Sprite* image1 = Sprite::create("Introduction/f1.png");
	image1->setPosition(Point(0, 0));
	m_pNodeImage->addChild(image1, 0, 1);

	Sprite* image2 = Sprite::create("Introduction/f2.png");
	image2->setPosition(Point(0, 960));
	m_pNodeImage->addChild(image2, 0, 2);

	Sprite* image3 = Sprite::create("Introduction/f3.png");
	image3->setPosition(Point(0, 960*2));
	m_pNodeImage->addChild(image3, 0, 3);

	Sprite* image4 = Sprite::create("Introduction/f4.png");
	image4->setPosition(Point(0, 960*3));
	m_pNodeImage->addChild(image4, 0, 4);

	Sprite* image5 = Sprite::create("Introduction/f5.png");
	image5->setPosition(Point(0, 960*4));
	m_pNodeImage->addChild(image5, 0, 5);

	Sprite* image6 = Sprite::create("Introduction/f6.png");
	image6->setPosition(Point(0, 960*5));
	m_pNodeImage->addChild(image6, 0, 6);

	m_pNodeImage->setPosition(winSize.width/2.0f, winSize.height/2.0f);
	this->addChild(m_pNodeImage);

	//create menu
	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Introduction/btn_close.png",
		"Introduction/btn_close.png",
		CC_CALLBACK_0(IntroductionLayer::menuCloseIntroduction, this));
	pCloseItem->setPosition(ccp(30, winSize.height-30));

	MenuItemImage* pNextItem = MenuItemImage::create(
		"Introduction/btn_next.png",
		"Introduction/btn_next.png",
		CC_CALLBACK_0(IntroductionLayer::menuNextImage, this));
	pNextItem->setPosition(ccp(winSize.width-30, winSize.height/2.0f + 30));

	MenuItemImage* pPrevItem = MenuItemImage::create(
		"Introduction/btn_prev.png",
		"Introduction/btn_prev.png",
		CC_CALLBACK_0(IntroductionLayer::menuPreviousImage, this));
	pPrevItem->setPosition(ccp(winSize.width-30, winSize.height/2.0f - 30));

	m_pMenuIntroduction = CCMenu::create(pCloseItem , pNextItem, pPrevItem, NULL);
	m_pMenuIntroduction->setPosition(CCPointZero);
	this->addChild(m_pMenuIntroduction);
	
	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	m_iImageTag = 1;

	return true;
}

void IntroductionLayer::menuCloseIntroduction()
{
	MainMenuScene* mainMenu =  MainMenuScene::create();
	Director::getInstance()->replaceScene(mainMenu);
}

void IntroductionLayer::menuNextImage()
{
	if (m_iImageTag < 6)
	{
		m_iImageTag++;
		auto actionMove = MoveBy::create(0.3f, Point(0, -960));
		m_pNodeImage->runAction(actionMove);
	}
}

void IntroductionLayer::menuPreviousImage()
{
	if (m_iImageTag > 1)
	{
		m_iImageTag--;
		auto actionMove = MoveBy::create(0.3f, Point(0, 960));
		m_pNodeImage->runAction(actionMove);
	}
}

bool IntroductionLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	return true;
}

void IntroductionLayer::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_pMenuIntroduction->isVisible())
	{
		m_pMenuIntroduction->setVisible(false);
	}
	else
	{
		m_pMenuIntroduction->setVisible(true);
	}
}