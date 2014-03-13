#include "FlashCardCollection.h"
#include "FlashCardScene.h"
#include "FooterNode.h"

USING_NS_CC;

bool FlashCardCollectionScene::init()
{
	if(CCScene::init())
	{
		this->_layer = FlashCardCollectionLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		return true;
	}
	else
	{
		return false;
	}
}

FlashCardCollectionScene::~FlashCardCollectionScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}

FlashCardCollectionLayer::~FlashCardCollectionLayer()
{
	
}

bool FlashCardCollectionLayer::init()
{
	if(!LayerColor::initWithColor(ccc4(255, 255, 255, 255)))
	{
		return false;
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	LabelTTF* pLabelTitle = LabelTTF::create("Flashcard Collection", "Arial", 32);
	pLabelTitle->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelTitle->setPosition(Point(320.0f, 900.0f));
	this->addChild(pLabelTitle);

	m_pFooterNode = FooterNode::create();
	m_pFooterNode->disableButtonIntroAndFlashCard();
	this->addChild(m_pFooterNode);

	m_pSlideShow = Node::create();
	m_pSlideShow->setContentSize(CCSizeMake(640.0f, 800));
	m_chapters = ChapterTable::getInstance()->getChaptersInfo();
	
	for (int iIndex=0; iIndex<m_chapters.size(); iIndex++)
	{
		Node* pNodeChapter = Node::create();
		ChapterInfo chapterInfo = m_chapters[iIndex];

		char sFilename[40];
		sprintf(sFilename, "FlashCard/chapter/%d.png", chapterInfo.iChapter%2+1);
		Sprite* pChapterImageSprite = Sprite::create(sFilename);
		pNodeChapter->addChild(pChapterImageSprite);

		char sTotalLevel[5];
		sprintf(sTotalLevel, "%d/%d", chapterInfo.iTotalLevelUnlock, 20);
		LabelTTF* pLabelTotalLevel = LabelTTF::create(sTotalLevel, "Arial", 22);
		pLabelTotalLevel->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelTotalLevel->setPositionY(-pChapterImageSprite->getContentSize().height/2.0f-12);
		pNodeChapter->addChild(pLabelTotalLevel);

		pNodeChapter->setAnchorPoint(Point(0.0f, 0.0f));
		switch(iIndex%4)
		{
		case 0:
			pNodeChapter->setPosition(Point(iIndex/4*640 + 170, 550.0f));
			break;
		case 1:
			pNodeChapter->setPosition(Point(iIndex/4*640 + 470, 550.0f));
			break;
		case 2:
			pNodeChapter->setPosition(Point(iIndex/4*640 + 170, 250.0f));
			break;
		case 3:
			pNodeChapter->setPosition(Point(iIndex/4*640 + 470, 250.0f));
			break;
		}
		m_pSlideShow->addChild(pNodeChapter);
	}

	m_pSlideShow->setPosition(Point(0.0f, 94.0f));
	this->addChild(m_pSlideShow);

	m_pButton1 = Sprite::create("FlashCard/chapter/mask.png");
	m_pButton1->setAnchorPoint(Point(0.5f, 0.0f));
	m_pButton1->setPosition(Point(170.0f, 530.0f));
	this->addChild(m_pButton1);  

	m_pButton2 = Sprite::create("FlashCard/chapter/mask.png");
	m_pButton2->setAnchorPoint(Point(0.5f, 0.0f));
	m_pButton2->setPosition(Point(470.0f, 530.0f));
	this->addChild(m_pButton2);

	m_pButton3 = Sprite::create("FlashCard/chapter/mask.png");
	m_pButton3->setAnchorPoint(Point(0.5f, 0.0f));
	m_pButton3->setPosition(Point(170.0f, 230.0f));
	this->addChild(m_pButton3);

	m_pButton4 = Sprite::create("FlashCard/chapter/mask.png");
	m_pButton4->setAnchorPoint(Point(0.5f, 0.0f));
	m_pButton4->setPosition(Point(470.0f, 230.0f));
	this->addChild(m_pButton4);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	if (m_chapters.size()%4 == 0)
	{
		m_iTotalPage = m_chapters.size()/4;
	}
	else
	{
		m_iTotalPage = m_chapters.size()/4 + 1;
	}
	
	m_iCurrentPage = 1;
	m_bIsSwipe = false;	
	Breadcrumb::getInstance()->addSceneMode(SceneMode::kFlashCardCollection);

	return true;
}

bool FlashCardCollectionLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if(m_pFooterNode->getSettingNode() != NULL && m_pFooterNode->getSettingNode()->getShowSetting())
	{
		return false;
	}

	m_fXMoved = 0;
	Point touchPosition = pTouch->getLocation();
	m_fBeginX = touchPosition.x;
	m_touchPositionMoved = touchPosition;
	m_bIsSwipe = false;
	m_iMoveLeftOrRight = 0;

	return true;
}

void FlashCardCollectionLayer::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();
	m_fXMoved += touchPosition.x - m_fBeginX;
	if ((m_fXMoved > 0 && m_iCurrentPage == 1) || (m_fXMoved < 0 && m_iCurrentPage == m_iTotalPage))
	{
		return;
	}

	if (m_touchPositionMoved.x > touchPosition.x)
		m_iMoveLeftOrRight = -1;
	else if (m_touchPositionMoved.x < touchPosition.x)
		m_iMoveLeftOrRight = 1;
	m_touchPositionMoved = touchPosition;

	auto actionMove = MoveBy::create(0.0f, Point(touchPosition.x - m_fBeginX, 0));
	m_pSlideShow->runAction(actionMove);
	m_fBeginX = touchPosition.x;

	if (m_fXMoved > 20 || m_fXMoved < -20)
	{
		m_bIsSwipe = true;
	}
}

void FlashCardCollectionLayer::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_bIsSwipe)
	{
		if ((m_fXMoved > 0 && m_iCurrentPage == 1) || (m_fXMoved < 0 && m_iCurrentPage == m_iTotalPage))
		{
			return;
		}

		if (m_iMoveLeftOrRight == 1 && m_fXMoved > 0)
		{
			auto actionMove = MoveBy::create(0.3f, Point(640 - m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
			m_iCurrentPage--;
		}
		else if (m_iMoveLeftOrRight == -1 && m_fXMoved < 0)
		{
			auto actionMove = MoveBy::create(0.3f, Point(-640 - m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
			m_iCurrentPage++;
		
		}
		else
		{
			auto actionMove = MoveBy::create(0.2f, Point(-m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
		}
	}
	else
	{
		if ((m_fXMoved > 0 && m_iCurrentPage != 1) || (m_fXMoved < 0 && m_iCurrentPage != m_iTotalPage))
		{
			auto actionMove = MoveBy::create(0.2f, Point(-m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
		}

		Point touchPosition = pTouch->getLocation();
		
		CCRect *pRectButton = new CCRect(m_pButton1->getOffsetPosition().x, 
			 m_pButton1->getOffsetPosition().y, 
			 m_pButton1->getTextureRect().size.width, 
			 m_pButton1->getTextureRect().size.height);
		Point touchButton = m_pButton1->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			CCLOG("button1");
			openFlashCard(1);
		}

		pRectButton = new CCRect(m_pButton1->getOffsetPosition().x, 
			 m_pButton2->getOffsetPosition().y, 
			 m_pButton2->getTextureRect().size.width, 
			 m_pButton2->getTextureRect().size.height);
		touchButton = m_pButton2->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			CCLOG("button2");
			openFlashCard(2);
		}

		pRectButton = new CCRect(m_pButton1->getOffsetPosition().x, 
			 m_pButton3->getOffsetPosition().y, 
			 m_pButton3->getTextureRect().size.width, 
			 m_pButton3->getTextureRect().size.height);
		touchButton = m_pButton3->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			CCLOG("button3");
			openFlashCard(3);
		}

		pRectButton = new CCRect(m_pButton1->getOffsetPosition().x, 
			 m_pButton4->getOffsetPosition().y, 
			 m_pButton4->getTextureRect().size.width, 
			 m_pButton4->getTextureRect().size.height);
		touchButton = m_pButton4->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			CCLOG("button4");
			openFlashCard(4);
		}
	}
	m_bIsSwipe = false;
}

void FlashCardCollectionLayer::openFlashCard(const int& iIndexButton)
{
	int iChapter = (m_iCurrentPage - 1)*4 + iIndexButton;
	if (iChapter <= m_chapters.size())
	{
		ChapterInfo chapterInfo = m_chapters[iChapter-1];
		if (chapterInfo.bIsUnlock)
		{
			FlashCardScene* pFlashCard = FlashCardScene::createScene(chapterInfo);
			Director::getInstance()->replaceScene(pFlashCard);
		}
	}
}