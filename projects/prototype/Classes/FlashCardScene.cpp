#include "FlashCardScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "GameWordManager.h"
#include "FooterNode.h"
#include "FlashCardNode.h"
#include "HeaderNode.h"

USING_NS_CC;

FlashCardScene* FlashCardScene::createScene(const ChapterInfo& chapterInfo)
{
	FlashCardScene* pFlashCardScene = new FlashCardScene();	  
	pFlashCardScene->m_chapterInfo = chapterInfo;
	if(pFlashCardScene->init())
	{
		pFlashCardScene->autorelease();
		return pFlashCardScene;
	}

	CC_SAFE_DELETE(pFlashCardScene);
	return NULL;
}

bool FlashCardScene::init()
{
	if(CCScene::init())
	{
		this->_layer = FlashCardLayer::createLayout(m_chapterInfo);
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

FlashCardLayer* FlashCardLayer::createLayout(const ChapterInfo& chapterInfo)
{
	FlashCardLayer* pFlashCardLayer = new FlashCardLayer();	  
	pFlashCardLayer->m_chapterInfo = chapterInfo;
	if(pFlashCardLayer->init())
	{
		pFlashCardLayer->autorelease();
		return pFlashCardLayer;
	}

	CC_SAFE_DELETE(pFlashCardLayer);
	return NULL;
}

bool FlashCardLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	Sprite* pBackground = Sprite::create("FlashCard/background.png");
	pBackground->setPosition(Point(320.0f, 480.0f));
	this->addChild(pBackground);

	HeaderNode* pHeaderNode = HeaderNode::create();
	this->addChild(pHeaderNode);

	WordlMapConfig worlMapConfig = GameConfigManager::getInstance()->GetWordlMapConfig();
	int iIndexChapter = worlMapConfig.m_WorlMapChapterConfigMap[m_chapterInfo.sChapterId];

	LabelBMFont *pLabelTitle = LabelBMFont::create(worlMapConfig.m_WorlMapChapterConfigs[iIndexChapter].m_sChapterName.c_str(), "fonts/font_title-popup.fnt");
	pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	this->addChild(pLabelTitle);

	char sTotalFlashCard[20];
	sprintf(sTotalFlashCard, "(%d/%d)", 1, m_chapterInfo.iTotalFlashCard);
	m_pLabelIndex = LabelTTF::create(sTotalFlashCard, "Arial", 30);
	m_pLabelIndex->setColor(ccc3(0.0f, 0.0f, 0.0f));

	m_pLabelIndex->setPosition(Point(320.0f + pLabelTitle->getContentSize().width/2.0f -2, 850.0f));
	pLabelTitle->setPosition(Point(320.0f - m_pLabelIndex->getContentSize().width/2.0f +2, 850.0f));

	this->addChild(m_pLabelIndex);

	m_pFooterNode = FooterNode::create();
	m_pFooterNode->removeButtonFlashcard();
	m_pFooterNode->removeBackground();
	this->addChild(m_pFooterNode);

	WordTable::getInstance()->refreshWordsForChapter(m_chapterInfo.sChapterId);
	
	m_Words = WordTable::getInstance()->getAllWordsForChapter(m_chapterInfo.sChapterId);
	m_iTotalFlashCardUnlock = m_chapterInfo.iTotalFlashCardUnlock;
	m_iIndexFlashCard = 1;

	m_pSlideShow = Node::create();
	m_pSlideShow->setContentSize(CCSizeMake(640.0f, 800));
	m_pSlideShow->setAnchorPoint(Point(0.0f, 0.0f));
	m_pSlideShow->setPosition(Point(0.0f, 94.0f));

	this->createNodeSlideShow();
	this->addChild(m_pSlideShow);

	ButtonManagerNode* pButtonManageNode = ButtonManagerNode::create();
	this->addChild(pButtonManageNode);

	Sprite* pIconLeft = Sprite::create("FlashCard/left_icon.png");
	m_pButtonLeft = ButtonNode::createButtonSprite(pIconLeft,  CC_CALLBACK_1(FlashCardLayer::clickButtonLeft, this));
	m_pButtonLeft->setPosition(Point(50.0f, 565.0f));
	pButtonManageNode->addButtonNode(m_pButtonLeft);
	m_pButtonLeft->setVisible(false);

	Sprite* pIconRight = Sprite::create("FlashCard/right_icon.png");
	m_pButtonRight = ButtonNode::createButtonSprite(pIconRight,  CC_CALLBACK_1(FlashCardLayer::clickButtonRight, this));
	m_pButtonRight->setPosition(Point(610.0f, 565.0f));
	pButtonManageNode->addButtonNode(m_pButtonRight);

	m_bMoveLeft = false;
	m_bMoveRight = false;

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	Breadcrumb::getInstance()->addSceneMode(SceneMode::kFlashCard);

	return true;
}

void FlashCardLayer::clickButtonLeft(Object* pSender)
{
	if (m_iIndexFlashCard > 1 && m_bMoveLeft == false && m_bMoveRight == false)
	{
		m_iIndexFlashCard--;
		auto actionCreateSlideShow = CallFunc::create(this, callfunc_selector(FlashCardLayer::createNodeSlideShow));
		auto actionMove = MoveBy::create(0.2f, Point(640, 0));
		m_pSlideShow->runAction(Sequence::create(actionMove, actionCreateSlideShow, NULL));
		m_bMoveLeft = true;
		m_pButtonRight->setVisible(true);
	}
	
	if (m_iIndexFlashCard == 1)
	{
		m_pButtonLeft->setVisible(false);
	}
}

void FlashCardLayer::clickButtonRight(Object* pSender)
{
	if (m_iIndexFlashCard < m_iTotalFlashCardUnlock && m_bMoveLeft == false && m_bMoveRight == false)
	{
		m_iIndexFlashCard++;
		auto actionCreateSlideShow = CallFunc::create(this, callfunc_selector(FlashCardLayer::createNodeSlideShow));
		auto actionMove = MoveBy::create(0.2f, Point(-640, 0));
		m_pSlideShow->runAction(Sequence::create(actionMove, actionCreateSlideShow, NULL));
		m_bMoveRight = true;
		m_pButtonLeft->setVisible(true);
	}

	if (m_iIndexFlashCard == m_iTotalFlashCardUnlock)
	{
		m_pButtonRight->setVisible(false);
	}
}

bool FlashCardLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
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

void FlashCardLayer::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();
	m_fXMoved += touchPosition.x - m_fBeginX;

	if ((m_fXMoved > 0 && m_iIndexFlashCard == 1) || (m_fXMoved < 0 && m_iIndexFlashCard == m_iTotalFlashCardUnlock))
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

void FlashCardLayer::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
	if (m_bIsSwipe)
	{
		if ((m_fXMoved > 0 && m_iIndexFlashCard == 1) || (m_fXMoved < 0 && m_iIndexFlashCard == m_iTotalFlashCardUnlock))
		{
			return;
		}

		auto actionCreateSlideShow = CallFunc::create(this, callfunc_selector(FlashCardLayer::createNodeSlideShow));

		if (m_iMoveLeftOrRight == 1 && m_fXMoved > 0)
		{
			m_iIndexFlashCard--;
			auto actionMove = MoveBy::create(0.3f, Point(640 - m_fXMoved, 0));
			m_pSlideShow->runAction(Sequence::create(actionMove, actionCreateSlideShow, NULL));
		}
		else if (m_iMoveLeftOrRight == -1 && m_fXMoved < 0)
		{
			m_iIndexFlashCard++;
			auto actionMove = MoveBy::create(0.3f, Point(-640 - m_fXMoved, 0));
			m_pSlideShow->runAction(Sequence::create(actionMove, actionCreateSlideShow, NULL));
		}
		else
		{	
			auto actionMove = MoveBy::create(0.2f, Point(-m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
		}
	}
	else
	{
		if ((m_fXMoved > 0 && m_iIndexFlashCard != 1) || (m_fXMoved < 0 && m_iIndexFlashCard != m_iTotalFlashCardUnlock))
		{
			auto actionMove = MoveBy::create(0.2f, Point(-m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
		}
	}

	if (m_iIndexFlashCard == 1)
	{
		m_pButtonLeft->setVisible(false);
	}
	else
	{
		m_pButtonLeft->setVisible(true);
	}

	if (m_iIndexFlashCard == m_iTotalFlashCardUnlock)
	{
		m_pButtonRight->setVisible(false);
	}
	else
	{
		m_pButtonRight->setVisible(true);
	}
}

void FlashCardLayer::createNodeSlideShow()
{
	m_pSlideShow->removeAllChildren();
 
	// Left
	if (m_iIndexFlashCard-2 >= 0)
	{
		WordInfo wordInfo = m_Words[m_iIndexFlashCard-2];
		int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(wordInfo.sWordId);
		auto& word = GameWordManager::getInstance()->GetWord(iIndex);

		FlashCardNode* pLastCardLeft = FlashCardNode::createLayout(word);
		pLastCardLeft->setPosition(Point(-640.0f, 0.0f));
		pLastCardLeft->addButtonPlaySoundWord();
		pLastCardLeft->addLayoutDescriptionWord();
		m_pSlideShow->addChild(pLastCardLeft);
	}

	
	// Mid
	if (m_iIndexFlashCard - 1 >= 0)
	{
		WordInfo wordInfo = m_Words[m_iIndexFlashCard-1];
		int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(wordInfo.sWordId);
		auto& word = GameWordManager::getInstance()->GetWord(iIndex);

		FlashCardNode* pLastCardCurrent = FlashCardNode::createLayout(word);
		pLastCardCurrent->setPosition(Point(0.0f, 0.0f));
		pLastCardCurrent->addButtonPlaySoundWord();
		pLastCardCurrent->addLayoutDescriptionWord();
		m_pSlideShow->addChild(pLastCardCurrent);
	}

	
	// Right
	if (m_iIndexFlashCard <= m_iTotalFlashCardUnlock)
	{
		WordInfo wordInfo = m_Words[m_iIndexFlashCard];
		int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(wordInfo.sWordId);
		auto& word = GameWordManager::getInstance()->GetWord(iIndex);

		FlashCardNode* pLastCardRight = FlashCardNode::createLayout(word);
		pLastCardRight->setPosition(Point(640.0f, 0.0f));
		pLastCardRight->addButtonPlaySoundWord();
		pLastCardRight->addLayoutDescriptionWord();
		m_pSlideShow->addChild(pLastCardRight);
	}

	char sTotaFlashCard[10];
	sprintf(sTotaFlashCard, "(%d/%d)", m_iIndexFlashCard, m_chapterInfo.iTotalFlashCard);
	m_pLabelIndex->setString(sTotaFlashCard);

	m_pSlideShow->setAnchorPoint(Point(0.0f, 0.0f));
	m_pSlideShow->setPosition(Point(0.0f, 0.0f));

	m_bMoveRight = false;
	m_bMoveLeft = false;
}