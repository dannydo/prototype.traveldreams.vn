#include "FlashCardScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "GameWordManager.h"
#include "DictionaryNode.h"
#include "FooterNode.h"
#include "SoundManager.h"

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
	if(!LayerColor::initWithColor(ccc4(255, 255, 255, 255)))
	{
		return false;
	}

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	/*
	LabelTTF* pLabelTitle = LabelTTF::create(m_chapterInfo.sName.c_str(), "Arial", 40);
	pLabelTitle->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelTitle->setPosition(Point(320.0f, 905.0f));
	this->addChild(pLabelTitle);
	*/

	char sTotalLevel[10];
	sprintf(sTotalLevel, "%d/%d", 1, m_chapterInfo.iTotalFlash);
	m_pLabelIndex = LabelTTF::create(sTotalLevel, "Arial", 22);
	m_pLabelIndex->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelIndex->setPosition(Point(320.0f, 870.0f));
	this->addChild(m_pLabelIndex);

	m_pFooterNode = FooterNode::create();
	m_pFooterNode->disableButtonIntroAndFlashCard();
	this->addChild(m_pFooterNode);

	if(m_chapterInfo.iCountFlashCardNew > 0)
	{
		WordTable::getInstance()->refreshWordsForChapter(m_chapterInfo.sChapterId);
	}

	m_Words = WordTable::getInstance()->getAllWordsForChapter(m_chapterInfo.sChapterId);
	m_iTotalFlashCard = m_chapterInfo.iTotalFlashCardUnlock;
	m_iIndexFlashCard = 1;

	m_pSlideShow = Node::create();
	m_pSlideShow->setContentSize(CCSizeMake(640.0f, 800));
	m_pSlideShow->setAnchorPoint(Point(0.0f, 0.0f));
	m_pSlideShow->setPosition(Point(0.0f, 94.0f));
	m_pSlideShow->setVisible(false);

	this->createNodeSlideShow();
	this->addChild(m_pSlideShow);

	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

	Breadcrumb::getInstance()->addSceneMode(SceneMode::kFlashCard);
   
	m_pIconSoundSenetenceSprite = Sprite::create("FlashCard/icon_sound.png");
	m_pIconSoundSenetenceSprite->setPosition(Point(500.0f, 300.0f));
	m_pIconSoundSenetenceSprite->setVisible(false);
	this->addChild(m_pIconSoundSenetenceSprite);

	m_pIconSoundWordSprite = Sprite::create("FlashCard/icon_sound.png");
	m_pIconSoundWordSprite->setPosition(Point(500.0f, 730.0f));
	m_pIconSoundWordSprite->setVisible(false);
	this->addChild(m_pIconSoundWordSprite);

	return true;
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

	if ((m_fXMoved > 0 && m_iIndexFlashCard == 1) || (m_fXMoved < 0 && m_iIndexFlashCard == m_iTotalFlashCard))
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
		if ((m_fXMoved > 0 && m_iIndexFlashCard == 1) || (m_fXMoved < 0 && m_iIndexFlashCard == m_iTotalFlashCard))
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
		if ((m_fXMoved > 0 && m_iIndexFlashCard != 1) || (m_fXMoved < 0 && m_iIndexFlashCard != m_iTotalFlashCard))
		{
			auto actionMove = MoveBy::create(0.2f, Point(-m_fXMoved, 0));
			m_pSlideShow->runAction(actionMove);
		}

		Point touchPosition = pTouch->getLocation();
		
		CCRect *pRectButton = new CCRect(m_pIconSoundSenetenceSprite->getOffsetPosition().x, 
				m_pIconSoundSenetenceSprite->getOffsetPosition().y, 
				m_pIconSoundSenetenceSprite->getTextureRect().size.width, 
				m_pIconSoundSenetenceSprite->getTextureRect().size.height);
		Point touchButton = m_pIconSoundSenetenceSprite->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			playVoiceSentence();
		}

		pRectButton = new CCRect(m_pIconSoundWordSprite->getOffsetPosition().x, 
				m_pIconSoundWordSprite->getOffsetPosition().y, 
				m_pIconSoundWordSprite->getTextureRect().size.width, 
				m_pIconSoundWordSprite->getTextureRect().size.height);
		touchButton = m_pIconSoundWordSprite->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			playVoiceWord();
		}

		pRectButton = new CCRect(m_pLabelWord->getOffsetPosition().x, 
				m_pLabelWord->getOffsetPosition().y, 
				m_pLabelWord->getTextureRect().size.width, 
				m_pLabelWord->getTextureRect().size.height);
		touchButton = m_pLabelWord->convertToNodeSpace(touchPosition);
		if(pRectButton->containsPoint(touchButton))
		{
			openDictionary(m_pLabelWord->getString());
		}
	}
}

void FlashCardLayer::playVoiceWord()
{
	SoundManager::PlaySpellingOfWord(this, m_currentWord);
}

void FlashCardLayer::playVoiceSentence()
{
	SoundManager::PlaySentenceOfWord(this, m_currentWord);
}

void FlashCardLayer::openDictionary(const char* sWord)
{
	DictionaryNode* pDictionary = DictionaryNode::createLayout(sWord);
	pDictionary->setPosition(Point(320.0f, 680.0f));
	this->getParent()->addChild(pDictionary, 20);
}

void FlashCardLayer::createNodeSlideShow()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	Node* pLastCardLeft = this->createLayoutFlashCard(m_iIndexFlashCard - 1);
	Node* pLastCardCurrent = this->createLayoutFlashCard(m_iIndexFlashCard);
	Node* pLastCardRight = this->createLayoutFlashCard(m_iIndexFlashCard + 1);

	m_pSlideShow->removeAllChildren();
	char sTotaFlashCard[10];
	sprintf(sTotaFlashCard, "%d/%d", m_iIndexFlashCard, m_chapterInfo.iTotalFlash);
	m_pLabelIndex->setString(sTotaFlashCard);

	Node* pNodeDisplay = this->createLayoutFlashCard(m_iIndexFlashCard);
	pNodeDisplay->setAnchorPoint(Point(0.0f, 0.0f));
	pNodeDisplay->setPosition(Point(0.0f, 0.0f));

	m_pSlideShow->setAnchorPoint(Point(0.0f, 0.0f));
	m_pSlideShow->setPosition(Point(0.0f, 94.0f));
	m_pSlideShow->setVisible(true);
	
	if(pLastCardLeft != NULL)
	{
		pLastCardLeft->setAnchorPoint(Point(0.0f, 0.0f));
		pLastCardLeft->setPosition(Point(-640.0f, 0.0f));
		m_pSlideShow->addChild(pLastCardLeft);
	}

	if(pLastCardCurrent != NULL)
	{
		pLastCardCurrent->setAnchorPoint(Point(0.0f, 0.0f));
		pLastCardCurrent->setPosition(Point(0.0f, 0.0f));
		m_pSlideShow->addChild(pLastCardCurrent);
	}

	if(pLastCardRight != NULL)
	{
		pLastCardRight->setAnchorPoint(Point(0.0f, 0.0f));
		pLastCardRight->setPosition(Point(640.0f, 0.0f));
		m_pSlideShow->addChild(pLastCardRight);
	}

	m_currentWordInfo = m_Words[m_iIndexFlashCard-1];
	m_pLabelWord = LabelTTF::create(m_currentWordInfo.sWordId.c_str(), "Arial", 32.0f);
	m_pLabelWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelWord->setPosition(Point(320.0f, 650.0f));
	m_pLabelWord->setVisible(false);
	m_pSlideShow->addChild(m_pLabelWord);

	if(m_currentWordInfo.sWordId != "")
	{
		int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(m_currentWordInfo.sWordId); //GetWordIndexFromContent(m_currentLevelInfo.sWordKey);
		m_currentWord = GameWordManager::getInstance()->GetWord(iIndex);

		// Update data when user view flash card new
		if (m_currentWordInfo.bIsNew == true)
		{
			m_currentWordInfo.bIsNew = false;
			m_chapterInfo.iCountFlashCardNew--;

			WordTable::getInstance()->updateWord(m_currentWordInfo);
			ChapterTable::getInstance()->updateChapter(m_chapterInfo);
			m_Words = WordTable::getInstance()->getAllWordsForChapter(m_chapterInfo.sChapterId);
		}
	}
}

Node* FlashCardLayer::createLayoutFlashCard(const int& iIndexFlashCard)
{
	if (iIndexFlashCard < 1 || iIndexFlashCard > m_iTotalFlashCard)
	{
		return NULL;
	}

	Node* pNode = Node::create();

	Sprite* pBackgroundBorder = Sprite::create("FlashCard/backgroundborder.png");
	pBackgroundBorder->setPosition(Point(320.0f, 400.0f));
	pNode->addChild(pBackgroundBorder);

	WordInfo wordInfo = m_Words[iIndexFlashCard-1];
	if(wordInfo.iCountCollected > 1)
	{	
		int iIndex = GameWordManager::getInstance()->GetLoadedWordIndexFromID(wordInfo.sWordId);
		Word word = GameWordManager::getInstance()->GetWord(iIndex);

		Sprite* pBackgroundFlashCardBorder = Sprite::create("FlashCard/flashcardbackgroundborder.png");
		pBackgroundFlashCardBorder->setPosition(Point(320.0f, 400.0f));
		pNode->addChild(pBackgroundFlashCardBorder);

		std::string sPath = GameWordManager::getInstance()->GetPackagePathFromWord(word);;
		sPath.append("/FlashCard/");
		sPath.append(word.m_sFlashCardImage);
		Sprite* pImageFlashcard = Sprite::create(sPath.c_str());
		pImageFlashcard->setPosition(Point(320.0f, 430.0f));
		pNode->addChild(pImageFlashcard);

		LabelTTF* pLabelWord = LabelTTF::create(word.m_sWord, "Arial", 32.0f);
		pLabelWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWord->setPosition(Point(320.0f, 650.0f));
		pNode->addChild(pLabelWord);

		LabelTTF* pLabelWordPhonetic = LabelTTF::create(word.m_sPhonetic.c_str(), "Arial", 20.0f);
		pLabelWordPhonetic->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWordPhonetic->setPosition(Point(320.0f, 620.0f));
		pNode->addChild(pLabelWordPhonetic);

		LabelTTF* pLabelWordMean = LabelTTF::create(word.m_sMeaning.c_str(), "Arial", 25.0f);
		pLabelWordMean->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWordMean->setPosition(Point(320.0f, 310.0f));
		pNode->addChild(pLabelWordMean);

		LabelTTF* pLabelWordSentence = LabelTTF::create(word.m_sSentence.c_str(), "Arial", 25.0f, Size(300.0f, 135.0f), TextHAlignment::CENTER, TextVAlignment::TOP);
		pLabelWordSentence->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWordSentence->setAnchorPoint(Point(0.5, 1.0f));
		pLabelWordSentence->setPosition(Point(320.0f, 235.0f));
		pNode->addChild(pLabelWordSentence);

		Sprite* pIconSoundSenetenceSprite = Sprite::create("FlashCard/icon_sound.png");
		pIconSoundSenetenceSprite->setPosition(Point(500.0f, 636.0f));
		pNode->addChild(pIconSoundSenetenceSprite);

		Sprite* pIconSoundWordSprite = Sprite::create("FlashCard/icon_sound.png");
		pIconSoundWordSprite->setPosition(Point(500.0f, 206.0f));
		pNode->addChild(pIconSoundWordSprite);
	}
	else
	{
		return NULL;
	}

	return pNode;
}