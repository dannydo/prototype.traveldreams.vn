#include "FooterNode.h"
#include "IntroductionLayer.h"
#include "FlashCardCollection.h"
#include "Database\WordTable.h"

USING_NS_CC;

FooterNode::~FooterNode()
{
	
}

bool FooterNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	m_pBackground = Sprite::create("Footer/Footer.png");
	m_pBackground->setAnchorPoint(Point(0.0f, 0.0f));
	m_pBackground->setPosition(Point(0.0f, 0.0f));
	this->addChild(m_pBackground);

	Sprite* pSettingSprite = Sprite::create("Footer/btn_setting.png");
	m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, CC_CALLBACK_1(FooterNode::openSettingMenu, this));
	m_pButtonSettingNode->setPosition(Point(36.0f, 33.0f));

	Sprite* pFlashCardSprite = Sprite::create("Footer/flashcard_button.png");

	int iWordNew = WordTable::getInstance()->getNumberWordNew();

	m_pIconNotiy = Sprite::create("Footer/noitify_msg.png");
	m_pIconNotiy->setPosition(Point(95.0f, 110.0f));

	char sNumberNew[10];
	sprintf(sNumberNew, "%d", iWordNew);
	m_pLabelNumber = LabelTTF::create(sNumberNew, "Arial", 25);
	m_pLabelNumber->setColor(ccc3(255.0f, 255.0f, 255.0f));
	m_pLabelNumber->setPosition(Point(22.0f, 20.0f));
	m_pIconNotiy->addChild(m_pLabelNumber);
	pFlashCardSprite->addChild(m_pIconNotiy);

	if (iWordNew < 1)
	{
		m_pIconNotiy->setVisible(false);
		m_pLabelNumber->setVisible(false);
	}

	int iTotalFlashCard = WordTable::getInstance()->getTotalWordCollected();
	char sTotalFlashCard[10];
	sprintf(sTotalFlashCard, "%d", iTotalFlashCard);
	m_pLabelTotalFlashWord = LabelTTF::create(sTotalFlashCard, "Arial", 30);
	m_pLabelTotalFlashWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelTotalFlashWord->setPosition(Point(61.0f, 50.0f));
	pFlashCardSprite->addChild(m_pLabelTotalFlashWord);

	m_pButtonFlashCardNode = ButtonNode::createButtonSprite(pFlashCardSprite, CC_CALLBACK_1(FooterNode::openFlashCardCollection, this));
	m_pButtonFlashCardNode->setPosition(Point(583.0f, 58.0f));

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->addButtonNode(m_pButtonSettingNode);
	m_pButtonManagerNode->addButtonNode(m_pButtonFlashCardNode);

	this->addChild(m_pButtonManagerNode);

	m_pSettingNode = NULL;

	auto actioneAddSettingNode = CallFunc::create(this, callfunc_selector(FooterNode::addSettingNode));
	this->runAction(Sequence::create(DelayTime::create(0.01), actioneAddSettingNode, NULL));
	
	return true;
}

void FooterNode::openFlashCardCollection(Object* sender)
{
	if (m_pButtonFlashCardNode->isVisible())
	{
		FlashCardCollectionScene* pFlashCardCollection = FlashCardCollectionScene::create();
		Director::getInstance()->replaceScene(pFlashCardCollection);
	}
}

void FooterNode::addSettingNode()
{
	m_pSettingNode = SettingMenuNode::create();
	m_pSettingNode->setPosition(Point(-505.0f, 0.0f));
	m_pSettingNode->addButtonSetting(m_pButtonSettingNode);
	this->getParent()->addChild(m_pSettingNode);
}

void FooterNode::openSettingMenu(Object* sender)
{
	if(m_pSettingNode == NULL)
	{
		m_pSettingNode = SettingMenuNode::create();
		m_pSettingNode->setPosition(Point(-505.0f, 0.0f));
		m_pSettingNode->addButtonSetting(m_pButtonSettingNode);
		this->getParent()->addChild(m_pSettingNode);
	}

	if (m_pSettingNode->getShowSetting() == false)
	{
		m_pSettingNode->show();
	}
	else
	{
		m_pSettingNode->hide();
	}
}

void FooterNode::removeButtonFlashcard()
{
	m_pButtonManagerNode->removeButtonNode(m_pButtonFlashCardNode);
}

void FooterNode::removeBackground()
{
	this->removeChild(m_pBackground);
}

void FooterNode::changeStatusButtonFlashcard(StatusButtonFlashcard statusButtonFlashcard)
{
	m_pButtonManagerNode->removeButtonNode(m_pButtonFlashCardNode);

	Sprite* pFlashCardSprite = Sprite::create("Footer/flashcard_button.png");
	int iWordNew = WordTable::getInstance()->getNumberWordNew();
	if (iWordNew > 0)
	{
		m_pIconNotiy = Sprite::create("Footer/noitify_msg.png");
		m_pIconNotiy->setPosition(Point(95.0f, 110.0f));

		char sNumberNew[10];
		sprintf(sNumberNew, "%d", iWordNew);
		m_pLabelNumber = LabelTTF::create(sNumberNew, "Arial", 25);
		m_pLabelNumber->setColor(ccc3(255.0f, 255.0f, 255.0f));
		m_pLabelNumber->setPosition(Point(22.0f, 20.0f));
		m_pIconNotiy->addChild(m_pLabelNumber);
		pFlashCardSprite->addChild(m_pIconNotiy);
	}

	int iTotalFlashCard = WordTable::getInstance()->getTotalWordCollected();
	char sTotalFlashCard[10];
	sprintf(sTotalFlashCard, "%d", iTotalFlashCard);
	m_pLabelTotalFlashWord = LabelTTF::create(sTotalFlashCard, "Arial", 30);
	m_pLabelTotalFlashWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
	m_pLabelTotalFlashWord->setPosition(Point(61.0f, 50.0f));
	pFlashCardSprite->addChild(m_pLabelTotalFlashWord);

	switch (statusButtonFlashcard)
	{
		case StatusButtonFlashcard::eNoClick :
		{
			pFlashCardSprite->setPosition(Point(590.0f, 50.0f));
			pFlashCardSprite->setScale(0.85f);
			this->addChild(pFlashCardSprite);
			break;
		}
		case StatusButtonFlashcard::eClickShowFlashcardCollection:
		{
			m_pButtonFlashCardNode = ButtonNode::createButtonSprite(pFlashCardSprite, CC_CALLBACK_1(FooterNode::openFlashCardCollection, this));
			m_pButtonFlashCardNode->setPosition(Point(583.0f, 58.0f));
			m_pButtonFlashCardNode->setPosition(Point(609.0f, 33.0f));
			m_pButtonManagerNode->addButtonNode(m_pButtonFlashCardNode);
			break;
		}
		default:
		{
			m_pButtonFlashCardNode = ButtonNode::createButtonSprite(pFlashCardSprite, CC_CALLBACK_1(FooterNode::openFlashCardCollection, this));
			m_pButtonFlashCardNode->setPosition(Point(583.0f, 58.0f));
			m_pButtonFlashCardNode->setPosition(Point(609.0f, 33.0f));
			m_pButtonManagerNode->addButtonNode(m_pButtonFlashCardNode);
			break;
		}
	}
}

void FooterNode::increaseTotalFlashcard(const int& iIncrease)
{
	char sTotalFlashCard[10];
	int iTotalFlashCard = WordTable::getInstance()->increaseTotalWordCollected(iIncrease);
	sprintf(sTotalFlashCard, "%d", iTotalFlashCard);
	m_pLabelTotalFlashWord->setString(sTotalFlashCard);
}

void FooterNode::updateLayoutWordNew()
{
	int iWordNew = WordTable::getInstance()->getNumberWordNew();
	if (iWordNew > 0)
	{
		char sNumberNew[10];
		sprintf(sNumberNew, "%d", iWordNew);
		m_pIconNotiy->setVisible(true);
		m_pLabelNumber->setVisible(true);
		m_pLabelNumber->setString(sNumberNew);
	}
	else
	{
		m_pIconNotiy->setVisible(false);
		m_pLabelNumber->setVisible(false);
	}
}