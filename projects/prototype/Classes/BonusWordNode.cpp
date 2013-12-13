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

	Sprite *pBackground = Sprite::create("Bonus-Word/Bonus_Board.png");
	this->addChild(pBackground);

	m_pSubWordNodes = new Array();
	m_pSubWordNodes->initWithCapacity(1);
 
	for(int iIndex=0; iIndex<m_subWord.size(); iIndex++)
	{
		Node* pNode = Node::create();

		Word word = m_subWord[iIndex];
		int width = 0;
		Node* pNodeWord = Node::create();
		int len = strlen(word.m_sWord);
		for(int i=0; i<len; i++)
		{
			char c[1];
			c[0] = word.m_sWord[i];

			Label* pLabelNone = Label::createWithTTF("_", "fonts/ARLRDBD.ttf", 30);
			Label* pLabelSubWord = Label::createWithTTF(c, "fonts/ARLRDBD.ttf", 30);
			float delta = abs(pLabelNone->getContentSize().width - pLabelSubWord->getContentSize().width)/2.0f;

			pLabelNone->setColor(ccc3(0, 0, 0));
			pNodeWord->addChild(pLabelNone, 1, len+i+1);

			pLabelSubWord->setColor(ccc3(0, 0, 0));
			pNodeWord->addChild(pLabelSubWord, 1, i+1);
			pLabelSubWord->setVisible(false);

			if (pLabelNone->getContentSize().width > pLabelSubWord->getContentSize().width)
			{
				pLabelNone->setPosition(Point(width, 0));
				pLabelSubWord->setPosition(Point(width + delta + 3, 0));
				width = width + pLabelNone->getContentSize().width + 3;
			}
			else
			{
				pLabelNone->setPosition(Point(width + delta, 0));
				pLabelSubWord->setPosition(Point(width + 3, 0));
				width = width + pLabelSubWord->getContentSize().width + 3;
			}
		}
		pNodeWord->setPosition(Point(-width/2.0f, 15));
		pNode->addChild(pNodeWord, 1, iIndex);

		Label* pLabelMeaningSubWord = Label::createWithTTF(word.m_sMeaning.c_str(), "fonts/HelveticaNeueLTCom-Ex.ttf", 20);
		pLabelMeaningSubWord->setColor(ccc3(187, 187, 187));
		pLabelMeaningSubWord->setPosition(Point(-pLabelMeaningSubWord->getContentSize().width/2.0f, 0));
		pNode->addChild(pLabelMeaningSubWord);

		pNode->setPosition(Point(0, 38 - iIndex*75));
		this->addChild(pNode);
		m_pSubWordNodes->addObject(pNode);
	}

	auto actionScale = ScaleTo::create(0.0f, 1.0f, (pBackground->getContentSize().height*m_subWord.size()/3.0f)/pBackground->getContentSize().height);
	pBackground->runAction(actionScale);
	pBackground->setPosition(Point(0, (pBackground->getContentSize().height - pBackground->getContentSize().height*m_subWord.size()/3.0f))/2.0f);

	return true;
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
				Node* pNode = (Node*)m_pSubWordNodes->getObjectAtIndex(iIndex);
				Node* pNodeWord = pNode->getChildByTag(iIndex);
				Node* lblChar = pNodeWord->getChildByTag(i+1);
				if (lblChar->isVisible() == false)
				{
					lblChar->setVisible(true);
				}
			}
		}
	}
}

/*
void BonusWordNode::setVisibleWordNodes(const bool& bVisible)
{
	int count = m_subWordEnable.size();
	for(int iIndex=0; iIndex<m_pSubWordNodes->count(); iIndex++)
	{
		bool isSetVisible = true;
		for(int i=0; i<m_subWordEnable.size(); i++)
		{
			if (m_subWordEnable[i] == iIndex)
			{
				isSetVisible = false;
				break;
			}
		}

		if (isSetVisible)
		{
			Node* pNode = (Node*)m_pSubWordNodes->getObjectAtIndex(iIndex);
			pNode->setVisible(bVisible);
			pNode->setPosition(Point(0, 38 - count*75));
			count++;
		}
	}
}
*/