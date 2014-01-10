#include "FlashCardScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"

USING_NS_CC;

bool FlashCardScene::init()
{
	if(CCScene::init())
	{
		this->_layer =FlashCardLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

FlashCardScene::~FlashCardScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

FlashCardLayer::~FlashCardLayer()
{
	
}

bool FlashCardLayer::init()
{
	if(!Node::init())
	{
		return false;
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	m_iTotalFlashCard = 4;
	m_iIndexFlashCard = 1;
	m_bRunningEffect = false;
	m_bShowSlideShow = false;

	m_pFlashCardCollection = Sprite::create("FlashCard/flashcards-collection.jpg");
	m_pFlashCardCollection->setAnchorPoint(Point(0.5f, 1.0f));
	m_pFlashCardCollection->setPosition(Point(winSize.width/2.0f, winSize.height));
	this->addChild(m_pFlashCardCollection);

	m_pFlashCardDisplay = Node::create();
	this->addChild(m_pFlashCardDisplay);

	m_pSlideShow = Node::create();
	this->addChild(m_pSlideShow);

	Sprite* pBarBottom = Sprite::create("FlashCard/bar-bottom.jpg");
	pBarBottom->setAnchorPoint(Point(0.5f, 0.0f));
	pBarBottom->setPosition(Point(winSize.width/2.0f, 0));
	this->addChild(pBarBottom);

	CCMenuItemImage* pBackItem = CCMenuItemImage::create(
		"World-Map/mask-button-back.png",
		"World-Map/mask-button-back.png",
		CC_CALLBACK_0(FlashCardLayer::menuBackCallBack, this));
	pBackItem->setPosition(ccp(56, 49));

	CCMenuItemImage* pFlashCard = CCMenuItemImage::create(
	"World-Map/mask-button-flash-card.png",
	"World-Map/mask-button-flash-card.png",
	CC_CALLBACK_0(FlashCardLayer::menuCloseSlideShowFlashCardCallBack, this));
	pFlashCard->setPosition(ccp(365, 52));

	CCMenuItemImage* pCollectionSchool = CCMenuItemImage::create(
	"FlashCard/mask.png",
	"FlashCard/mask.png",
	CC_CALLBACK_0(FlashCardLayer::showSlideShowFlashCard, this));
	pCollectionSchool->setPosition(ccp(188, 775));

	CCMenu* pMenu = CCMenu::create(pBackItem, pFlashCard, pCollectionSchool, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
   
	return true;
}

void FlashCardLayer::setNameClassParent(const char* sNameClassParent)
{
	m_sNameClassParent = sNameClassParent;
}

void FlashCardLayer::menuBackCallBack()
{
	if (m_sNameClassParent == "WorldMapScene")
	{
		WorldMapScene* pWordMap = WorldMapScene::create();
		Director::getInstance()->replaceScene(pWordMap);
	}
	else if (m_sNameClassParent == "LevelMapScene")
	{
		LevelMapScene* pLevelMap = LevelMapScene::create();
		Director::getInstance()->replaceScene(pLevelMap);
	}
}

bool FlashCardLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_bShowSlideShow == true)
	{
		m_fXMoved = 0;
		Point touchPosition = pTouch->getLocation();
		m_fBeginX = touchPosition.x;
	}

	return true;
}

void FlashCardLayer::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_bRunningEffect == false && m_bShowSlideShow == true)
	{
		Point touchPosition = pTouch->getLocation();
		m_fXMoved += touchPosition.x - m_fBeginX;

		if ((m_fXMoved > 0 && m_iIndexFlashCard == 1) || (m_fXMoved < 0 && m_iIndexFlashCard == m_iTotalFlashCard))
		{
			return;
		}

		m_pSlideShow->setVisible(true);
		m_pFlashCardDisplay->setVisible(false);

		auto actionMove = MoveBy::create(0.0f, Point(touchPosition.x - m_fBeginX, 0));
		m_pSlideShow->runAction(actionMove);
		m_fBeginX = touchPosition.x;
	}
}

void FlashCardLayer::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_bRunningEffect == false && m_bShowSlideShow == true)
	{
		if ((m_fXMoved > 0 && m_iIndexFlashCard == 1) || (m_fXMoved < 0 && m_iIndexFlashCard == m_iTotalFlashCard))
		{
			return;
		}

		m_bRunningEffect == true;
		auto actionCreateSlideShow = CallFunc::create(this, callfunc_selector(FlashCardLayer::createNodeSlideShow));

		if (m_fXMoved > 0)
		{
			m_iIndexFlashCard--;
			auto actionMove = MoveBy::create(0.3f, Point(690 - m_fXMoved, 0));
			m_pSlideShow->runAction(Sequence::create(actionMove, DelayTime::create(0.3f)->clone(), actionCreateSlideShow, NULL));
		}
		else if (m_fXMoved < 0)
		{
			m_iIndexFlashCard++;
			auto actionMove = MoveBy::create(0.3f, Point(-690 - m_fXMoved, 0));
			m_pSlideShow->runAction(Sequence::create(actionMove, DelayTime::create(0.3f)->clone(), actionCreateSlideShow, NULL));
		}
		else
		{
			m_pFlashCardDisplay->setVisible(true);
			m_pSlideShow->setVisible(false);
		}
	}
}

void FlashCardLayer::createNodeSlideShow()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	char sFileName[35];

	m_pSlideShow->removeAllChildren();
	m_pFlashCardDisplay->removeAllChildren();

	if (m_iIndexFlashCard > 1 && m_iIndexFlashCard < m_iTotalFlashCard)
	{
		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard-1);
		Sprite* pFlashCardLeft = Sprite::create(sFileName);
		pFlashCardLeft->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardLeft->setPosition(Point(-(winSize.width/2.0f + 50), winSize.height));
		m_pSlideShow->addChild(pFlashCardLeft);

		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard);
		Sprite* pFlashCardCurrent = Sprite::create(sFileName);
		pFlashCardCurrent->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardCurrent->setPosition(Point(winSize.width/2.0f, winSize.height));
		m_pSlideShow->addChild(pFlashCardCurrent);

		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard+1);
		Sprite* pFlashCardRight = Sprite::create(sFileName);
		pFlashCardRight->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardRight->setPosition(Point(winSize.width/2.0f + winSize.width + 50, winSize.height));
		m_pSlideShow->addChild(pFlashCardRight);
	}
	else if (m_iIndexFlashCard == 1)
	{
		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard);
		Sprite* pFlashCardCurrent = Sprite::create(sFileName);
		pFlashCardCurrent->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardCurrent->setPosition(Point(winSize.width/2.0f, winSize.height));
		m_pSlideShow->addChild(pFlashCardCurrent);

		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard+1);
		Sprite* pFlashCardRight = Sprite::create(sFileName);
		pFlashCardRight->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardRight->setPosition(Point(winSize.width/2.0f + winSize.width + 50, winSize.height));
		m_pSlideShow->addChild(pFlashCardRight);
	}
	else if (m_iIndexFlashCard == m_iTotalFlashCard)
	{
		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard-1);
		Sprite* pFlashCardLeft = Sprite::create(sFileName);
		pFlashCardLeft->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardLeft->setPosition(Point(-(winSize.width/2.0f + 50), winSize.height));
		m_pSlideShow->addChild(pFlashCardLeft);

		sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard);
		Sprite* pFlashCardCurrent = Sprite::create(sFileName);
		pFlashCardCurrent->setAnchorPoint(Point(0.5f, 1.0f));
		pFlashCardCurrent->setPosition(Point(winSize.width/2.0f, winSize.height));
		m_pSlideShow->addChild(pFlashCardCurrent);
	}

	sprintf(sFileName, "FlashCard/%d.jpg", m_iIndexFlashCard);
	Sprite* pFlashCard = Sprite::create(sFileName);
	pFlashCard->setAnchorPoint(Point(0.5f, 1.0f));
	pFlashCard->setPosition(Point(winSize.width/2.0f, winSize.height));
	m_pFlashCardDisplay->addChild(pFlashCard);

	m_pSlideShow->setPosition(Point::ZERO);
	m_pSlideShow->setVisible(false);
	m_pFlashCardDisplay->setVisible(true);
	m_bRunningEffect = false;
}

void FlashCardLayer::showSlideShowFlashCard()
{
	if (m_bShowSlideShow == false)
	{
		m_pFlashCardCollection->setVisible(false);
		m_bShowSlideShow = true;
		m_iIndexFlashCard = 1;
		this->createNodeSlideShow();
	}
}

void FlashCardLayer::menuCloseSlideShowFlashCardCallBack()
{
	m_pFlashCardCollection->setVisible(true);
	m_pSlideShow->setVisible(false);
	m_pFlashCardDisplay->setVisible(false);
	m_bShowSlideShow = false;
}