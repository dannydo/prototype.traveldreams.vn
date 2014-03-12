#include "FlashCardScene.h"
#include "WorldMapScene.h"
#include "LevelMapScene.h"
#include "GameWordManager.h"
#include "DictionaryNode.h"

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

	LabelTTF* pLabelTitle = LabelTTF::create(m_chapterInfo.sName.c_str(), "Arial", 40);
	pLabelTitle->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelTitle->setPosition(Point(320.0f, 905.0f));
	this->addChild(pLabelTitle);

	char sTotalLevel[10];
	sprintf(sTotalLevel, "%d/%d", 1, 20);
	m_pLabelIndex = LabelTTF::create(sTotalLevel, "Arial", 22);
	m_pLabelIndex->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelIndex->setPosition(Point(320.0f, 870.0f));
	this->addChild(m_pLabelIndex);

	Sprite* pBarBottom = Sprite::create("FlashCard/bar-bottom.jpg");
	pBarBottom->setAnchorPoint(Point(0.5f, 0.0f));
	pBarBottom->setPosition(Point(winSize.width/2.0f, 0));
	this->addChild(pBarBottom);

	CCMenuItemImage* pItemSettingMenu = CCMenuItemImage::create(
		"World-Map/mask-button-back.png",
		"World-Map/mask-button-back.png",
		CC_CALLBACK_0(FlashCardLayer::openStringMenu, this));
	pItemSettingMenu->setPosition(ccp(56, 49));

	CCMenu* pMenu = CCMenu::create(pItemSettingMenu, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu);

	m_levels = LevelTable::getInstance()->fetchLevelsForChapter(m_chapterInfo.iChapter);
	m_iTotalFlashCard = m_levels.size();
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

	m_pSettingNode = NULL;
	m_isShowSetting = false;
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

void FlashCardLayer::openStringMenu()
{
	if(m_pSettingNode == NULL)
	{
		m_pSettingNode = SettingMenuNode::create();
		m_pSettingNode->setPosition(Point(-500.0f, 0));
		this->addChild(m_pSettingNode);
	}

	if (m_isShowSetting == false)
	{
		m_isShowSetting = true;
		m_pSettingNode->show();
		this->setTouchEnabled(false);
	}
	else
	{
		m_isShowSetting = false;
		m_pSettingNode->hide();
		this->setTouchEnabled(true);
	}
}

bool FlashCardLayer::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)
{
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
	if (m_currentLevelInfo.bIsUnlock) 
	{
		char sSoundFile[100];
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			sprintf(sSoundFile, "EnglishSoundPC/Words/%s.wav", m_currentWordInfo.m_sSoundFile.c_str());
		#else
			sprintf(sSoundFile, "EnglishSound/Words/%s.ogg", m_currentWordInfo.m_sSoundFile.c_str());
		#endif
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);
	}
}

void FlashCardLayer::playVoiceSentence()
{
	if (m_currentLevelInfo.bIsUnlock) 
	{
		char sSoundFile[100];
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			sprintf(sSoundFile, "EnglishSoundPC/Sentences/%s.wav", m_currentWordInfo.m_sSentenceSoundFile.c_str());
		#else
			sprintf(sSoundFile, "EnglishSound/Sentences/%s.ogg", m_currentWordInfo.m_sSentenceSoundFile.c_str());
		#endif
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);
	}
}

void FlashCardLayer::openDictionary(const char* sWord)
{
	if (m_currentLevelInfo.bIsUnlock) 
	{
		DictionaryNode* pDictionary = DictionaryNode::createLayout(sWord);
		pDictionary->setPosition(Point(320.0f, 680.0f));
		this->getParent()->addChild(pDictionary, 20);
	}
}

void FlashCardLayer::createNodeSlideShow()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	Node* pLastCardLeft = this->createLayoutFlashCard(m_iIndexFlashCard - 1);
	Node* pLastCardCurrent = this->createLayoutFlashCard(m_iIndexFlashCard);
	Node* pLastCardRight = this->createLayoutFlashCard(m_iIndexFlashCard + 1);

	m_pSlideShow->removeAllChildren();
	char sTotalLevel[10];
	sprintf(sTotalLevel, "%d/%d", m_iIndexFlashCard, 20);
	m_pLabelIndex->setString(sTotalLevel);

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

	m_currentLevelInfo = m_levels[m_iIndexFlashCard-1];
	m_pLabelWord = LabelTTF::create(m_currentLevelInfo.sWordKey.c_str(), "Arial", 32.0f);
	m_pLabelWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelWord->setPosition(Point(320.0f, 650.0f));
	m_pLabelWord->setVisible(false);
	m_pSlideShow->addChild(m_pLabelWord);

	if(m_currentLevelInfo.sWordKey != "")
	{
		int iIndex = GameWordManager::getInstance()->GetWordIndexFromContent(m_currentLevelInfo.sWordKey);
		m_currentWordInfo = GameWordManager::getInstance()->GetWord(iIndex);
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

	LevelInfo levelInfo = m_levels[iIndexFlashCard-1];
	if(levelInfo.bIsUnlock)
	{	
		int iIndex = GameWordManager::getInstance()->GetWordIndexFromContent(levelInfo.sWordKey);
		Word wordInfo = GameWordManager::getInstance()->GetWord(iIndex);

		Sprite* pBackgroundFlashCardBorder = Sprite::create("FlashCard/flashcardbackgroundborder.png");
		pBackgroundFlashCardBorder->setPosition(Point(320.0f, 400.0f));
		pNode->addChild(pBackgroundFlashCardBorder);

		std::string sPath = "FlashCard/flashcardimage/";
		sPath.append(wordInfo.m_sFlashCardImage);
		Sprite* pImageFlashcard = Sprite::create(sPath.c_str());
		pImageFlashcard->setPosition(Point(320.0f, 430.0f));
		pNode->addChild(pImageFlashcard);

		LabelTTF* pLabelWord = LabelTTF::create(wordInfo.m_sWord, "Arial", 32.0f);
		pLabelWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWord->setPosition(Point(320.0f, 650.0f));
		pNode->addChild(pLabelWord);

		LabelTTF* pLabelWordPhonetic = LabelTTF::create(wordInfo.m_sPhonetic.c_str(), "Arial", 20.0f);
		pLabelWordPhonetic->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWordPhonetic->setPosition(Point(320.0f, 620.0f));
		pNode->addChild(pLabelWordPhonetic);

		LabelTTF* pLabelWordMean = LabelTTF::create(wordInfo.m_sMeaning.c_str(), "Arial", 25.0f);
		pLabelWordMean->setColor(ccc3(0.0f, 0.0f, 0.0f));
		pLabelWordMean->setPosition(Point(320.0f, 310.0f));
		pNode->addChild(pLabelWordMean);

		LabelTTF* pLabelWordSentence = LabelTTF::create(wordInfo.m_sSentence.c_str(), "Arial", 25.0f, Size(300.0f, 135.0f), TextHAlignment::CENTER, TextVAlignment::TOP);
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
		LabelTTF* pLabelWarning = LabelTTF::create("Flashcard do not unlock.", "Arial", 32.0f);
		pLabelWarning->setColor(ccc3(255.0f, 0.0f, 0.0f));
		pLabelWarning->setPosition(Point(320.0f, 400.0f));
		pNode->addChild(pLabelWarning);
	}

	return pNode;
}