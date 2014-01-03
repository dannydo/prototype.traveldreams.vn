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

	m_pFlashCardDisplay = Node::create();
	this->addChild(m_pFlashCardDisplay);

	m_pSlideShow = Node::create();
	this->createNodeSlideShow();
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

	CCMenu* pMenu = CCMenu::create(pBackItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	/*
	m_pBackground = Sprite::create("FlashCard/background.png");
	m_pBackground->setOpacity(0);
	this->addChild(m_pBackground);

	std::string sPath = "FlashCard/";
	sPath.append(m_word.m_sFlashCardImage);
	m_pFlashCard = Sprite::create(sPath.c_str());
	m_pFlashCard->setPosition(Point(96.0f, 3.0f));
	m_pFlashCard->setOpacity(0);
	this->addChild(m_pFlashCard);

	Node* pWord = Node::create();
	m_pTitleWord = LabelTTF::create(m_word.m_sWord, "Arial", 25);
	m_pTitleWord->setColor(ccc3(13, 118, 200));
	m_pTitleWord->setPositionY(10);
	m_pTitleWord->setOpacity(0);
	pWord->addChild(m_pTitleWord);

	m_pMeaningWord = LabelTTF::create(m_word.m_sMeaning.c_str(), "Arial", 18);
	m_pMeaningWord->setColor(ccc3(102, 102, 102));
	m_pMeaningWord->setPositionY(-30);
	m_pMeaningWord->setOpacity(0);
	pWord->addChild(m_pMeaningWord);

	pWord->setContentSize(CCSizeMake(106.0f, 180.0f));
	pWord->setPosition(-108.0f, 3.0f);
	this->addChild(pWord);
	*/

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
	m_fXMoved = 0;
	Point touchPosition = pTouch->getLocation();
	m_fBeginX = touchPosition.x;
	return true;
}

void FlashCardLayer::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_bRunningEffect == false)
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
	if (m_bRunningEffect == false)
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