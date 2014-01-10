#include "BonusWordNodeNew.h"
#include "SimpleAudioEngine.h"
#include "SoundManager.h"

USING_NS_CC;

BonusWordNodeNew* BonusWordNodeNew::createLayout(const std::vector<Word> bonusWord)
{
	BonusWordNodeNew* pBonusWord = new BonusWordNodeNew();
	pBonusWord->m_bonusWords = bonusWord;

	if(pBonusWord->init())
	{
		pBonusWord->autorelease();
		return pBonusWord;
	}

	

	CC_SAFE_DELETE(pBonusWord);
	return NULL;
}

BonusWordNodeNew::~BonusWordNodeNew()
{
	if (m_LabelLetterDisplays)
	{
		m_LabelLetterDisplays->release();
		m_LabelLetterDisplays = NULL;
	}
}

bool BonusWordNodeNew::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_LabelLetterDisplays = new Array();
	m_LabelLetterDisplays->initWithCapacity(0);

	m_pMarks = new Array();
	m_pMarks->initWithCapacity(0);

	int iHeight = 0;
	
	for (int iIndex=0; iIndex<m_bonusWords.size(); iIndex++)
	{
		Node* pNode = this->createNodeBonusWord(m_bonusWords[iIndex]);
		pNode->setPosition(Point(0, iHeight));
		iHeight += pNode->getContentSize().height + 25;
		pNode->setVisible(false);
		this->addChild(pNode, 0, iIndex);
		m_CompletedUnlockBonusWprds.push_back(0);
		m_CompletedUnlockBonusWprds.push_back(0);
	}

	this->setAnchorPoint(Point(0.5f, 0.0f));
	this->scheduleUpdate();
	m_fDeltaUpdate = 0.0f;
	m_fTimeAutoHiddenPopup = 3.0f;
	m_bRunningCollectWord = false;
	m_pEffecFirstUnlockLetter = NULL;

	m_pBackgroundEffectLayer = LayerColor::create(ccc4( 0,0,0, 100));
	m_pBackgroundEffectLayer->setVisible(false);
	this->addChild(m_pBackgroundEffectLayer);

	return true;
}

void BonusWordNodeNew::update(float dt)
{
	m_fDeltaUpdate += dt;
	if (m_fDeltaUpdate >= m_fTimeAutoHiddenPopup && m_bRunningCollectWord == false)
	{
		this->closeBonusWord();
		m_fDeltaUpdate = 0.0f;
	}
}

void BonusWordNodeNew::addLetter(const unsigned char letter)
{
	m_letters.push_back(letter);
	m_pBackgroundEffectLayer->setPosition(Point(-this->getPositionX(), -this->getPositionY()));
}

float BonusWordNodeNew::displayEffect(const float& fTimeDelay)
{
	m_iCountWord = -1;
	m_bRunningCollectWord = false;
	float fDelay = this->calculatorDelayTime();
	
	auto actionUpdateLetter = CallFunc::create(this, callfunc_selector(BonusWordNodeNew::updateLetterCollectForWord));
	auto delay = DelayTime::create(fTimeDelay);
	this->runAction(Sequence::create(delay->clone(), actionUpdateLetter, NULL));
	 
	return fDelay;
}

float BonusWordNodeNew::calculatorDelayTime()
{
	float fDelay = 0.0f;
	
	for(int iIndexWord=0; iIndexWord<m_bonusWords.size(); iIndexWord++)
	{
		std::vector<int> letters = m_WordLetterIndex[iIndexWord];

		Word word = m_bonusWords[iIndexWord];
		bool bShowBonusWord = true;
		for(int iIndex=0; iIndex<strlen(word.m_sWord); iIndex++)
		{
			bool isFirstUnlockLetter = true;
			for(int iIndexWordLetter=0; iIndexWordLetter<m_WordLetterIndex[iIndexWord].size(); iIndexWordLetter++)
			{
				if (m_WordLetterIndex[iIndexWord][iIndexWordLetter] == 1)
				{
					isFirstUnlockLetter = false;
					break;
				}
			}

			for(int iIndexLetter=0; iIndexLetter<m_letters.size(); iIndexLetter++)
			{
				if (tolower(m_letters[iIndexLetter]) == tolower(word.m_sWord[iIndex]) && m_WordLetterIndex[iIndexWord][iIndex] == 0)
				{
					if (isFirstUnlockLetter)
					{
						if (bShowBonusWord)
						{
							bShowBonusWord = false;
							fDelay += 0.95f;
							fDelay += 1.65f;
						}
						else
						{
							fDelay += 0.25f;
						}
						letters[iIndex] = 1;
					}
					else
					{
						if (bShowBonusWord)
						{
							bShowBonusWord = false;
							fDelay += 0.7f;
						}
						else
						{
							fDelay += 0.25f;
						}
						letters[iIndex] = 1;
					}
				}
			}
		}

		if (m_CompletedUnlockBonusWprds[iIndexWord] == 0)
		{
			bool isFinish = true;
			for(int iIndex=0; iIndex<letters.size(); iIndex++)
			{
				if(letters[iIndex] == 0)
				{
					isFinish = false;
					break;
				}
			}

			if (isFinish)
			{
				m_CompletedUnlockBonusWprds[iIndexWord] = 1;
				fDelay += 3.4f + word.m_fSoundLength;
			}
		}
	}

	return fDelay;
}

void BonusWordNodeNew::updateLetterCollectForWord()
{
	if (m_bonusWords.size() < 1)
	{
		return;
	}

	if (m_bRunningCollectWord == false)
	{
		m_iCountWord++;
		m_iCountLetterWord = 0;
		m_iCountLetter = 0;

		this->removeChild(m_pEffecFirstUnlockLetter);
		m_pEffecFirstUnlockLetter = NULL;
	}

	if (m_iCountWord > m_bonusWords.size() - 1)
	{
		this->emptyArrayLetter();
		return;
	}

	Word word = m_bonusWords[m_iCountWord];
	Node* pNode = this->getChildByTag(m_iCountWord);
	Node* pNodeWord = pNode->getChildByTag(0)->getChildByTag(0)->getChildByTag(0);
	bool isDelay = false;
	bool isFirstDelay = false;
	m_bRunningCollectWord = true;

	if (m_iCountLetterWord < strlen(word.m_sWord))
	{
		if (m_iCountLetter < m_letters.size())
		{
			if (tolower(m_letters[m_iCountLetter]) == tolower(word.m_sWord[m_iCountLetterWord]) && m_WordLetterIndex[m_iCountWord][m_iCountLetterWord] == 0)
			{
				isDelay = true;
				if (pNode->isVisible() == false)
				{
					if (m_pEffecFirstUnlockLetter == NULL)
					{
						isFirstDelay = true;
					}
					m_WordLetterIndex[m_iCountWord][m_iCountLetterWord] = 1;
					this->playEffectUnlockFirstLetter(m_iCountLetterWord, (LabelTTF*)pNodeWord->getChildByTag(m_iCountLetterWord));
				}
				else
				{
					Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(m_iCountWord);
					if (pMark->getTag() == -1)
					{
						isFirstDelay = true;
					}
					m_WordLetterIndex[m_iCountWord][m_iCountLetterWord] = 1;
					this->playEffectLetter((LabelTTF*)pNodeWord->getChildByTag(m_iCountLetterWord));
				}
			}

			m_iCountLetter++;
		}
		else
		{
			m_iCountLetter = 0;
			m_iCountLetterWord++;
		}
	}
	else
	{
		m_bRunningCollectWord = false;
		this->updateLetterDisplay();
		if (pNode->isVisible() == false && m_pEffecFirstUnlockLetter != NULL)
		{
			auto actionScale = ScaleBy::create(0.7f, 0.0f, 0.0f);
			auto actionMove = MoveTo::create(0.7f, pNode->getPosition());
			auto actionSetVisibelNode = CallFunc::create(this, callfunc_selector(BonusWordNodeNew::setVisibleTrueLayoutBonusWord));

			m_pEffecFirstUnlockLetter->runAction(Sequence::create(DelayTime::create(0.7f)->clone(), actionScale, NULL));
			m_pEffecFirstUnlockLetter->runAction(Sequence::create(DelayTime::create(0.7f)->clone(), actionMove, actionSetVisibelNode, NULL));

			isFirstDelay = true;
			isDelay = true;
		}
		else
		{
			Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(m_iCountWord);
			if (pMark->getTag() != -1)
			{
				pMark->setTag(-1);
				auto actionMove = MoveTo::create(0.2f, Point(-pMark->getContentSize().width, 0));
				pMark->runAction(actionMove);
			}
		}
	}

	if (isDelay)
	{
		DelayTime* delay;
		if (pNode->isVisible() == false)
		{
			if (isFirstDelay)
			{
				if (m_bRunningCollectWord)
				{
					delay = DelayTime::create(0.95f);
				}
				else
				{
					delay = DelayTime::create(1.65f);
				}
			}
			else
			{
				if (m_bRunningCollectWord)
				{
					delay = DelayTime::create(0.45f);
				}
				else
				{
					delay = DelayTime::create(1.65f);
				}
			}
		}
		else
		{
			Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(m_iCountWord);
			if (isFirstDelay)
			{
				delay = DelayTime::create(0.7f);
			}
			else
			{
				delay = DelayTime::create(0.25f);
			}
		}

		auto actionLoopUpdate = CallFunc::create(this, callfunc_selector(BonusWordNodeNew::updateLetterCollectForWord));
		if (this->checkFinishCollectWord(m_iCountWord))
		{
			if (pNode->isVisible() == false && m_pEffecFirstUnlockLetter != NULL)
			{
				auto actionScale = ScaleBy::create(0.7f, 0.0f, 0.0f);
				auto actionMove = MoveTo::create(0.7f, pNode->getPosition());
				auto actionSetVisibelNode = CallFunc::create(this, callfunc_selector(BonusWordNodeNew::setVisibleTrueLayoutBonusWord));

				m_pEffecFirstUnlockLetter->runAction(Sequence::create(DelayTime::create(0.7f)->clone(), actionScale, NULL));
				m_pEffecFirstUnlockLetter->runAction(Sequence::create(DelayTime::create(0.7f)->clone(), actionMove, actionSetVisibelNode, NULL));
				delay = DelayTime::create(1.65f);
			}

			auto actionEffect = CallFunc::create(this, callfunc_selector(BonusWordNodeNew::playEffectFinishCollectWord));
			auto delayEffect = DelayTime::create(3.4f +  word.m_fSoundLength);
			this->runAction(Sequence::create(delay->clone(), actionEffect, delayEffect->clone(), actionLoopUpdate, NULL));
		}
		else
		{
			this->runAction(Sequence::create(delay->clone(), actionLoopUpdate, NULL));
		}
	}
	else
	{
		this->updateLetterCollectForWord();
	}
}

void BonusWordNodeNew::playEffectLetter(LabelTTF* pLabel)
{
	Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(m_iCountWord);
	if (pMark->getTag() == -1)
	{
		pMark->setTag(m_iCountWord);
		auto actionMove = MoveTo::create(0.2f, Point(-9, 0));
		pMark->runAction(actionMove);

		auto actionSacleEffect = ScaleBy::create(0.1f, 2.0f, 2.0f);
		pLabel->runAction(Sequence::create(DelayTime::create(0.3f)->clone(), actionSacleEffect, actionSacleEffect->reverse(), NULL));
	}
	else
	{
		auto actionSacleEffect = ScaleBy::create(0.1f, 2.0f, 2.0f);
		pLabel->runAction(Sequence::create(actionSacleEffect, actionSacleEffect->reverse(), NULL));
	}

	pLabel->setColor(ccc3(0, 0, 0));
}

bool  BonusWordNodeNew::checkFinishCollectWord(const int& iIndexWord)
{
	for(int iIndex=0; iIndex<m_WordLetterIndex[iIndexWord].size(); iIndex++)
	{
		if(m_WordLetterIndex[iIndexWord][iIndex] == 0)
		{
			return false;
		}
	}

	return true;
}

void BonusWordNodeNew::playEffectFinishCollectWord()
{
	m_pBackgroundEffectLayer->setVisible(true);
	Word word = m_bonusWords[m_iCountWord];
	Size winSize = Director::sharedDirector()->getWinSize();
	Node* pNode = this->getChildByTag(m_iCountWord);
	Node* pNodeWord = this->getChildByTag(m_iCountWord)->getChildByTag(0)->getChildByTag(0)->getChildByTag(0);
	pNodeWord->setVisible(false);

	m_pLabelEffectFinish = LabelTTF::create("", "Arial", 90);
	m_pLabelEffectFinish->setString(word.m_sWord);
	m_pLabelEffectFinish->setScale(0.3f);
	m_pLabelEffectFinish->setPosition(Point(m_pLabelEffectFinish->getContentSize().width/2.0f*0.3, pNode->getPositionY()));
	m_pLabelEffectFinish->setColor(ccc3(255, 255, 255));
	
	this->addChild(m_pLabelEffectFinish);

	auto actionSacle = ScaleBy::create(1.0f, 3.0f, 3.0f);
	auto actionRemove = CallFunc::create(this, callfunc_selector(BonusWordNodeNew::removeLabelFinishColectWord));
	auto actionFadeOut = FadeOut::create(1.0f);
	auto actionMove = MoveTo::create(1.0f, Point(320-this->getPositionX(), -300));
	m_pLabelEffectFinish->runAction(Sequence::create(actionSacle, DelayTime::create(1.0f)->clone(), actionFadeOut, actionRemove, NULL));
	m_pLabelEffectFinish->runAction(actionMove);

	Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(m_iCountWord);
	if (pMark->getTag() != -1)
	{
		pMark->setTag(-1);
		auto actionMove = MoveTo::create(0.2f, Point(-pMark->getContentSize().width, 0));
		pMark->runAction(actionMove);
	}

	char sSoundFile[40];
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		sprintf(sSoundFile, "EnglishSoundPC/%s.wav", word.m_sSoundFile.c_str());
	#else
		sprintf(sSoundFile, "EnglishSound/%s.ogg", word.m_sSoundFile.c_str());
	#endif
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);
}

void BonusWordNodeNew::removeLabelFinishColectWord()
{
	this->removeChild(m_pLabelEffectFinish);
	Node* pNodeWord = this->getChildByTag(m_iCountWord)->getChildByTag(0)->getChildByTag(0)->getChildByTag(0);
	pNodeWord->setVisible(true);
	m_pBackgroundEffectLayer->setVisible(false);
}

void BonusWordNodeNew::emptyArrayLetter()
{
	while(!m_letters.empty())
	{
		m_letters.pop_back();
	}
}

Node* BonusWordNodeNew::createNodeBonusWord(Word word)
{
	Node* pLayoutNode = Node::create();

	Node* pNodeWord = Node::create();

	char letter[2];
	letter[0] = toupper(word.m_sWord[0]);
	letter[1] = 0;
	LabelTTF* pLabelLetter = LabelTTF::create(letter, "Arial", 24);
	pLabelLetter->setColor(ccc3(0, 0, 0));
	m_LabelLetterDisplays->addObject(pLabelLetter);
	
	Sprite* pDisplay = Sprite::create("Bonus-Word/background.png");
	pLabelLetter->setPosition(Point(pDisplay->getContentSize().width/2.0f, pDisplay->getContentSize().height/2.0f));
	pDisplay->addChild(pLabelLetter);

	MenuItem* pMenuItem = MenuItemSprite::create(
		pDisplay,
		pDisplay,
		CC_CALLBACK_1(BonusWordNodeNew::popupBonusWordCallBack, this));

	Menu* pMenuDisplay = Menu::create(pMenuItem, NULL);
	pMenuDisplay->setPosition(Point::ZERO);
	pLayoutNode->addChild(pMenuDisplay);
	pLayoutNode->setContentSize(pDisplay->getContentSize());

	int width = 0;
	std::vector<int> indexLetter;
	for(int iIndex=0; iIndex<strlen(word.m_sWord); iIndex++)
	{
		char letter[2];
		letter[0] = word.m_sWord[iIndex];
		letter[1] = 0;
		indexLetter.push_back(0);
		
		LabelTTF* pLabelLetter = LabelTTF::create(letter, "Arial", 24);
		pLabelLetter->setColor(ccc3(130, 130, 130));
		pLabelLetter->setPosition(Point(width + pLabelLetter->getContentSize().width/2.0f, 0));

		width = width + pLabelLetter->getContentSize().width + 2;
		pNodeWord->addChild(pLabelLetter, 0, iIndex);
	}

	m_WordLetterIndex.push_back(indexLetter);
	pNodeWord->setTag(0);
	pNodeWord->setPosition(Point::ZERO);

	Sprite* pBackground = Sprite::create("Bonus-Word/Bonus_Board_Small.png");
	pBackground->setPosition(Point(0, 0));
	pBackground->setScaleX((width + 24)/pBackground->getContentSize().width);
	pBackground->setPosition(Point(width/2.0f, 0));

	Node* pPopupWord = Node::create();
	pPopupWord->addChild(pBackground);
	pPopupWord->addChild(pNodeWord);
	pPopupWord->setPosition(Point(-8, 0));

	Sprite* m_pMark = Sprite::create("Bonus-Word/Bonus_Board_Small.png");
	m_pMark->setPosition(Point(0, 0));
	m_pMark->setTag(-1);
	m_pMark->setScaleX((width + 24)/m_pMark->getContentSize().width);
	m_pMark->setAnchorPoint(Point(0.0f, 0.5f));
	m_pMark->setPosition(Point(-m_pMark->getContentSize().width, 0));

	m_pMarks->addObject(m_pMark);
	
	ClippingNode* clipperMask = ClippingNode::create();
    clipperMask->addChild(pPopupWord, 0, 0);
    clipperMask->setStencil(m_pMark);
	clipperMask->setContentSize(m_pMark->getContentSize());
	pLayoutNode->addChild(clipperMask, 0, 0);

	return pLayoutNode;
}

void BonusWordNodeNew::updateLetterDisplay()
{
	Word word = m_bonusWords[m_iCountWord];
	bool bFInish = true;

	for (int iIndex=0; iIndex<m_WordLetterIndex[m_iCountWord].size(); iIndex++)
	{
		if (m_WordLetterIndex[m_iCountWord][iIndex] == 0)
		{
			bFInish = false;
			char letter[2];
			letter[0] = toupper(word.m_sWord[iIndex]);
			letter[1] = 0;
			LabelTTF* labelLetter =(LabelTTF*)m_LabelLetterDisplays->getObjectAtIndex(m_iCountWord);
			labelLetter->setString(letter);
			break;
		}
	}
															 
	if (bFInish)																	  
	{
		LabelTTF* labelLetter =(LabelTTF*)m_LabelLetterDisplays->getObjectAtIndex(m_iCountWord);
		labelLetter->setString("✓");
	}
}

void BonusWordNodeNew::popupBonusWordCallBack(Object* pSender)
{
	if (m_bRunningCollectWord == false)
	{
		m_fTimeAutoHiddenPopup = 3.0f;
		m_fDeltaUpdate = 0.0f;
		this->closeBonusWord();

		Node* pNode = (Node*)pSender;
		Node* pParent = pNode->getParent()->getParent();
		LabelTTF* pLabel = (LabelTTF*)m_LabelLetterDisplays->getObjectAtIndex(pParent->getTag());
		pLabel->setVisible(false);
		
		Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(pParent->getTag());
		if (pMark->getTag() == -1)
		{
			pMark->setTag(pParent->getTag());
			auto actionMove = MoveTo::create(0.2f, Point(-9, 0));
			auto actionMoveRevrse = MoveTo::create(0.2f, Point(-pMark->getContentSize().width, 0));
			auto actionUpdateTag =  CallFuncN::create(this, callfuncN_selector(BonusWordNodeNew::updateTagSpriteMark));
			pMark->runAction(Sequence::create(actionMove,  NULL));

			char sSoundFile[40];
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				sprintf(sSoundFile, "EnglishSoundPC/%s.wav", m_bonusWords[pParent->getTag()].m_sSoundFile.c_str());
			#else
				sprintf(sSoundFile, "EnglishSound/%s.ogg", m_bonusWords[pParent->getTag()].m_sSoundFile.c_str());
			#endif
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( sSoundFile);
		}
	}
}

void BonusWordNodeNew::updateTagSpriteMark(Node* pSender)
{
	Sprite* pSprite = (Sprite*)pSender;
	int iIndex = pSprite->getTag();
	if (iIndex < 0)
	{
		return;
	}
	LabelTTF* pLabel = (LabelTTF*)m_LabelLetterDisplays->getObjectAtIndex(iIndex);
	pLabel->setVisible(true);
	pSprite->setTag(-1);
}

void BonusWordNodeNew::closeBonusWord()
{
	for(int iIndex=0; iIndex<m_pMarks->count(); iIndex++)
	{
		Sprite* pMark = (Sprite*)m_pMarks->getObjectAtIndex(iIndex);
		if (pMark->getTag() != -1)
		{
			auto actionUpdateTag =  CallFuncN::create(this, callfuncN_selector(BonusWordNodeNew::updateTagSpriteMark));
			auto actionMove = MoveTo::create(0.2f, Point(-pMark->getContentSize().width, 0));
			pMark->runAction(Sequence::create(actionMove, actionUpdateTag, NULL));
		}
	}
}

void BonusWordNodeNew::playEffectUnlockFirstLetter(const int& iIndexLetterWord, LabelTTF* pLabelLetter)
{
	bool bIsDelay = false;
	if (m_pEffecFirstUnlockLetter == NULL)
	{
		m_pBackgroundEffectLayer->setVisible(true);
		bIsDelay= true;
		m_pEffecFirstUnlockLetter = Node::create();
		Word word = m_bonusWords[m_iCountWord];
		int width = 0;
		std::vector<int> indexLetter;
		for(int iIndex=0; iIndex<strlen(word.m_sWord); iIndex++)
		{
			char letter[2];
			letter[0] = word.m_sWord[iIndex];
			letter[1] = 0;
			indexLetter.push_back(0);
		
			LabelTTF* pLabelLetter = LabelTTF::create(letter, "Arial", 90);
			pLabelLetter->setColor(ccc3(4, 167, 252));
			pLabelLetter->setPosition(Point(width + pLabelLetter->getContentSize().width/2.0f, 0));
			width = width + pLabelLetter->getContentSize().width + 2;
			m_pEffecFirstUnlockLetter->addChild(pLabelLetter, 0, iIndex);
		}

		m_pEffecFirstUnlockLetter->setAnchorPoint(Point(0.5f, 0.5f));
		m_pEffecFirstUnlockLetter->setContentSize(CCSizeMake(width, 20));
		m_pEffecFirstUnlockLetter->setPosition(Point(320-this->getPositionX(), -300));
		m_pEffecFirstUnlockLetter->setScale(0.3f);
		this->addChild(m_pEffecFirstUnlockLetter);

		auto actionScale = ScaleBy::create(0.5f, 3.0f, 3.0f);
		m_pEffecFirstUnlockLetter->runAction(actionScale);

		// sound effect
		SoundManager::PlaySoundEffect(_SET_BONUS_WORD_APPEAR_);
	}

	if (m_pEffecFirstUnlockLetter != NULL)
	{
		pLabelLetter->setColor(ccc3(0, 0, 0));
		DelayTime* delay;
		if (bIsDelay)
		{
			delay = DelayTime::create(0.5f);
		}
		else
		{
			delay = DelayTime::create(0.0f);
		}

		LabelTTF* pLabel = (LabelTTF*)m_pEffecFirstUnlockLetter->getChildByTag(iIndexLetterWord);
		auto actionEffectColor =  CallFuncN::create(this, callfuncN_selector(BonusWordNodeNew::playEffectColorLetter));
		auto actionSacleEffect = ScaleBy::create(0.1f, 2.0f, 2.0f);
		pLabel->runAction(Sequence::create(delay->clone(), actionEffectColor, actionSacleEffect, actionSacleEffect->reverse(), NULL));
	}
}

void BonusWordNodeNew::playEffectColorLetter(Node* pSender)
{
	LabelTTF* pLabel = (LabelTTF*)pSender;
	pLabel->setColor(ccc3(255, 255, 255));
}

void BonusWordNodeNew::setVisibleTrueLayoutBonusWord()
{
	Node* pNode = this->getChildByTag(m_iCountWord);
	pNode->setVisible(true);
	m_pBackgroundEffectLayer->setVisible(false);
}