#include "BonusWordNode.h"

USING_NS_CC;

BonusWordNode* BonusWordNode::createLayout(const std::vector<Word> pSubWord)
{
	BonusWordNode* pBonusWord = new BonusWordNode();
	pBonusWord->m_subWord = pSubWord;

	if(pBonusWord->init())
	{
		pBonusWord->autorelease();
		return pBonusWord;
	}

	CC_SAFE_DELETE(pBonusWord);
	return NULL;
}

bool BonusWordNode::init()
{
	if (!Node::init())
	{
		return false;
	}
	
	this->setVisible(false);

	m_pLayoutNotificationNode = Node::create();
	Sprite *pBackgroundNotification = Sprite::create("Bonus-Word/Bonus_Board_Small.png");
	m_pLayoutNotificationNode->setContentSize(pBackgroundNotification->getContentSize());
	m_pLayoutNotificationNode->addChild(pBackgroundNotification, 1);

	Sprite *pBackground = Sprite::create("Bonus-Word/Bonus_Board.png");
	this->addChild(pBackground);
 
	for(int iIndex=0; iIndex<m_subWord.size(); iIndex++)
	{
		Node* pSubWordNode = this->createLayoutWord(m_subWord[iIndex]);
		pSubWordNode->setPosition(Point(0, 38 - iIndex*75));
		this->addChild(pSubWordNode, 1, iIndex);
		
		Node* pNodeNotification = this->createLayoutWord(m_subWord[iIndex]);
		pNodeNotification->setPosition(Point(0, -pBackgroundNotification->getContentSize().height/2.0f));
		pNodeNotification->setVisible(false);
		m_pLayoutNotificationNode->addChild(pNodeNotification, 1, iIndex);
	}

	auto actionScale = ScaleTo::create(0.0f, 1.0f, (pBackground->getContentSize().height*m_subWord.size()/3.0f)/pBackground->getContentSize().height);
	pBackground->runAction(actionScale);
	pBackground->setPosition(Point(0, (pBackground->getContentSize().height - pBackground->getContentSize().height*m_subWord.size()/3.0f))/2.0f);

	return true;
}

Node* BonusWordNode::createLayoutWord(const Word word)
{
	Node* pNode = Node::create();
	Node* pNodeWord = Node::create();
	int len = strlen(word.m_sWord);
	int width = 0;

	for(int i=0; i<len; i++)
	{
		char c[1];
		c[0] = word.m_sWord[i];

		Label* pLabelNone = Label::createWithTTF("_", "fonts/ARLRDBD.ttf", 30);
		Label* pLabelSubWord = Label::createWithTTF(c, "fonts/ARLRDBD.ttf", 30);
		float delta = abs(pLabelNone->getContentSize().width - pLabelSubWord->getContentSize().width)/2.0f;

		pLabelNone->setColor(ccc3(0, 0, 0));
		pNodeWord->addChild(pLabelNone);

		pLabelSubWord->setColor(ccc3(0, 0, 0));
		pLabelSubWord->setVisible(false);
		pNodeWord->addChild(pLabelSubWord, 1, i+1);

		if (pLabelNone->getContentSize().width > pLabelSubWord->getContentSize().width)
		{
			pLabelNone->setPosition(Point(width, 0));
			pLabelSubWord->setPosition(Point(width + delta + 4, 0));
			width = width + pLabelNone->getContentSize().width + 3;
		}
		else
		{
			pLabelNone->setPosition(Point(width + delta, 0));
			pLabelSubWord->setPosition(Point(width + 4, 0));
			width = width + pLabelSubWord->getContentSize().width + 3;
		}
	}

	pNodeWord->setPosition(Point(-width/2.0f, 15));
	pNode->addChild(pNodeWord, 2, 1);

	Label* pLabelMeaningSubWord = Label::createWithTTF(word.m_sMeaning.c_str(), "fonts/HelveticaNeueLTCom-Ex.ttf", 20);
	pLabelMeaningSubWord->setColor(ccc3(187, 187, 187));
	pLabelMeaningSubWord->setPosition(Point(-pLabelMeaningSubWord->getContentSize().width/2.0f, 0));
	pNode->addChild(pLabelMeaningSubWord, 2);

	return pNode;
}

void  BonusWordNode::addCharacterCollected(const char character)
{
	for(int iIndex=0; iIndex<m_subWord.size(); iIndex++)
	{
		Word word = m_subWord[iIndex];
		for(int i=0; i<strlen(word.m_sWord); i++)
		{
			if (character == word.m_sWord[i])
			{
				Node* pNode = this->getChildByTag(iIndex);
				Node* pNodeWord = pNode->getChildByTag(1);
				Node* lblChar = pNodeWord->getChildByTag(i+1);
				if (lblChar->isVisible() == false)
				{
					lblChar->setVisible(true);
				}
			}
		}
	}
}

void BonusWordNode::addCharacterToWordNotification(const char character)
{
	m_iNotification = 0;
	m_cCharacter = character;

	this->updateLayoutNotification();
}

void BonusWordNode::loopNotification()
{
	while(m_iNotification < m_subWord.size())
	{
		this->updateLayoutNotification();
	}
}

void BonusWordNode::updateLayoutNotification()
{
	Word word = m_subWord[m_iNotification];
	int isPlayEffect = false;

	for(int i=0; i<strlen(word.m_sWord); i++)
	{
		if (m_cCharacter == word.m_sWord[i])
		{
			this->setVisibleNotification(false);

			Node* pNode = m_pLayoutNotificationNode->getChildByTag(m_iNotification);
			pNode->setVisible(true);
			Node* pNodeWord = pNode->getChildByTag(1);
			Node* lblChar = pNodeWord->getChildByTag(i+1);
			if (lblChar->isVisible() == false)
			{
				isPlayEffect = true;
				lblChar->setVisible(true);
			}
		}
	}
	m_iNotification++;

	auto delay = DelayTime::create(0.4f);
	auto actionLoop = CallFunc::create(this, callfunc_selector(BonusWordNode::loopNotification));
	
	CCLOG("%f %f", m_pLayoutNotificationNode->getPositionX(), m_pLayoutNotificationNode->getPositionY());
	if (isPlayEffect)
	{
		auto actionMoveLeft = MoveTo::create(0.3f, Point(m_pLayoutNotificationNode->getContentSize().width/2.0f, m_pLayoutNotificationNode->getPositionY()));
		auto actionMoveRight = MoveTo::create(0.3f, Point(-m_pLayoutNotificationNode->getContentSize().width/2.0f, m_pLayoutNotificationNode->getPositionY()));
		
		CCLOG("%f", m_pLayoutNotificationNode->getContentSize().width/2.0f);
		CCLOG("%s", word.m_sWord);
		m_pLayoutNotificationNode->runAction(Sequence::create(actionMoveLeft, delay->clone(), actionMoveRight, delay->clone(), actionLoop, NULL));
	}
	else
	{
		this->runAction(Sequence::create(actionLoop, NULL));
	}
}

void BonusWordNode::setVisibleNotification(const bool& bVisible)
{
	for(int iIndex=0; iIndex<m_subWord.size(); iIndex++)
	{
		if (iIndex != m_iNotification)
		{
			Node* pNode = m_pLayoutNotificationNode->getChildByTag(iIndex);
			pNode->setVisible(bVisible);
		}
	}
}

Node* BonusWordNode::createButtonShowPopupBonus()
{
	Node* pNode = Node::create();
	MenuItemImage* pCloseItem = MenuItemImage::create(
		"Bonus-Word/Bonus_button.png",
		"Bonus-Word/Bonus_button_click.png",
		CC_CALLBACK_0(BonusWordNode::menuShowPopupCallBack, this));

	Menu* menuPopup = Menu::create(pCloseItem, NULL);
	pNode->addChild(menuPopup, 10);

	return pNode;
}

void BonusWordNode::menuShowPopupCallBack()
{
	if (this->isVisible())
	{
		this->setVisible(false);
	}
	else
	{
		this->setVisible(true);
	}
}