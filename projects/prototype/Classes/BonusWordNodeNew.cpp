#include "BonusWordNodeNew.h"

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
	
	for (int iIndex=0; iIndex<m_bonusWords.size(); iIndex++)
	{
		Node* pNode = this->createNodeBonusWord(m_bonusWords[iIndex]);
		pNode->setPosition(Point(iIndex*80+pNode->getContentSize().width/2.0f, 0));
		this->addChild(pNode, 0, iIndex);
		this->setContentSize(CCSizeMake(iIndex*80 + pNode->getContentSize().width, pNode->getContentSize().height));
	}		

	this->setAnchorPoint(Point(0.5, 0));
	this->scheduleUpdate();
	m_fDeltaUpdate = 0.0f;
	m_fTimeAutoHiddenPopup = 3.0f;
	m_bRunningCollectWord = false;

	return true;
}

void BonusWordNodeNew::update(float dt)
{
	m_fDeltaUpdate += dt;
	if (m_fDeltaUpdate >= m_fTimeAutoHiddenPopup && m_bRunningCollectWord == false)
	{
		this->setVisibleWordNode(false);
		m_fDeltaUpdate = 0.0f;
	}
}

void BonusWordNodeNew::addLetter(const unsigned char letter)
{
	for (int iIndexWord=0; iIndexWord<m_bonusWords.size(); iIndexWord++)
	{
		Word word = m_bonusWords[iIndexWord];
		Node* pNodeWord = this->getChildByTag(iIndexWord)->getChildByTag(0)->getChildByTag(0);

		for (int iIndex=0; iIndex<m_WordLetterIndex[iIndexWord].size(); iIndex++)
		{
			if (tolower(letter) == tolower(word.m_sWord[iIndex]) && m_WordLetterIndex[iIndexWord][iIndex] == 0)
			{
				m_WordLetterIndex[iIndexWord][iIndex] = 1;
				LabelTTF* plabelLetter = (LabelTTF*)pNodeWord->getChildByTag(iIndex);
				plabelLetter->setColor(ccc3(0, 0, 0));
			}
		}

		this->updateLetterDisplay(iIndexWord);
	}
}

Node* BonusWordNodeNew::createNodeBonusWord(Word word)
{
	Node* pLayoutNode = Node::create();

	Node* pNodeWord = Node::create();
	Sprite* pBackground = Sprite::create("Bonus-Word/Bonus_Board_Small.png");
	pBackground->setPosition(Point(0, 9));
	
	int width = 0;
	std::vector<int> indexLetter;
	for(int i=0; i<strlen(word.m_sWord); i++)
	{
		char letter[2];
		letter[0] = word.m_sWord[i];
		letter[1] = 0;
		indexLetter.push_back(0);
		
		LabelTTF* pLabelLetter = LabelTTF::create(letter, "Arial", 18);
		pLabelLetter->setColor(ccc3(130, 130, 130));
		pLabelLetter->setPosition(Point(width + pLabelLetter->getContentSize().width/2.0f, 0));

		width = width + pLabelLetter->getContentSize().width + 2;
		pNodeWord->addChild(pLabelLetter, 0, i);
	}

	m_WordLetterIndex.push_back(indexLetter);
	pNodeWord->setTag(0);
	pNodeWord->setPosition(Point(-width/2.0f, 20));
	
	LabelTTF* pLabelMeaning = LabelTTF::create(word.m_sMeaning.c_str(), "Arial", 14);
	pLabelMeaning->setColor(ccc3(130, 130, 130));
	pLabelMeaning->setPosition(Point(0, -8));

	if (width < pLabelMeaning->getContentSize().width)
	{
		width = pLabelMeaning->getContentSize().width;
	}

	auto actionScale = ScaleBy::create(0.0f, (width + 24)/pBackground->getContentSize().width , 0.65);
	pBackground->runAction(actionScale);

	Node* pPopupWord = Node::create();
	pPopupWord->addChild(pBackground);
	pPopupWord->addChild(pLabelMeaning);
	pPopupWord->addChild(pNodeWord);
	pPopupWord->setPosition(Point(0, 40));
	pPopupWord->setVisible(false);
	
	pLayoutNode->addChild(pPopupWord, 0, 0);

	char letter[2];
	letter[0] = toupper(word.m_sWord[0]);
	letter[1] = 0;
	LabelTTF* pLabelLetter = LabelTTF::create(letter, "Arial", 28);
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

	return pLayoutNode;
}

void BonusWordNodeNew::updateLetterDisplay(int iIndexWord)
{
	Word word = m_bonusWords[iIndexWord];
	bool bFInish = true;

	for (int iIndex=0; iIndex<m_WordLetterIndex[iIndexWord].size(); iIndex++)
	{
		if (m_WordLetterIndex[iIndexWord][iIndex] == 0)
		{
			bFInish = false;
			char letter[2];
			letter[0] = toupper(word.m_sWord[iIndex]);
			letter[1] = 0;
			LabelTTF* labelLetter =(LabelTTF*)m_LabelLetterDisplays->getObjectAtIndex(iIndexWord);
			labelLetter->setString(letter);
			break;
		}
	}

	if (bFInish)
	{
		LabelTTF* labelLetter =(LabelTTF*)m_LabelLetterDisplays->getObjectAtIndex(iIndexWord);
		labelLetter->setString("✓");
	}
}

void BonusWordNodeNew::popupBonusWordCallBack(Object* pSender)
{
	m_fTimeAutoHiddenPopup = 3.0f;
	m_fDeltaUpdate = 0.0f;

	Node* pNode = (Node*)pSender;
	Node* pPopupWord = pNode->getParent()->getParent()->getChildByTag(0);
	
	if (pPopupWord->isVisible())
	{
		pPopupWord->setVisible(false);
		this->setVisibleWordNode(false);
	}
	else
	{
		this->setVisibleWordNode(false);
		pPopupWord->setVisible(true);
	}
}

void BonusWordNodeNew::setVisibleWordNode(const bool& bVisible)
{
	for (int iIndexWord=0; iIndexWord<m_bonusWords.size(); iIndexWord++)
	{
		this->getChildByTag(iIndexWord)->getChildByTag(0)->setVisible(bVisible);
	}
}