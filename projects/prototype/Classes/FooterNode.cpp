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
	Sprite* pSettingSpriteActive = Sprite::create("Footer/btn_setting.png");
	m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, pSettingSpriteActive, CC_CALLBACK_1(FooterNode::openSettingMenu, this));
	m_pButtonSettingNode->setPosition(Point(36.0f, 33.0f));

	Sprite* pFlashCardSprite = Sprite::create("Footer/flashcard_button.png");
	int iWordNew = WordTable::getInstance()->getNumberWordNew();
	if (iWordNew > 0)
	{
		Sprite* pIconNotiy = Sprite::create("Footer/noitify_msg.png");
		pIconNotiy->setPosition(Point(95.0f, 110.0f));

		char sNumberNew[10];
		sprintf(sNumberNew, "%d", iWordNew);
		LabelTTF* pLabelNumber = LabelTTF::create(sNumberNew, "Arial", 25);
		pLabelNumber->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelNumber->setPosition(Point(22.0f, 20.0f));
		pIconNotiy->addChild(pLabelNumber);
		pFlashCardSprite->addChild(pIconNotiy);
	}

	int iTotalFlashCard = WordTable::getInstance()->getNumberWordCollected();
	char sTotalFlashCard[10];
	sprintf(sTotalFlashCard, "%d", iTotalFlashCard);
	LabelTTF* pLabelTotalFlashWord = LabelTTF::create(sTotalFlashCard, "Arial", 30);
	pLabelTotalFlashWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelTotalFlashWord->setPosition(Point(61.0f, 50.0f));
	pFlashCardSprite->addChild(pLabelTotalFlashWord);

	m_pButtonFlashCardNode = ButtonNode::createButtonSprite(pFlashCardSprite, CC_CALLBACK_1(FooterNode::openFlashCardCollection, this));
	m_pButtonFlashCardNode->setPosition(Point(583.0f, 58.0f));

	m_pButtonManagerNode = ButtonManagerNode::create();
	m_pButtonManagerNode->addButtonNode(m_pButtonSettingNode);
	m_pButtonManagerNode->addButtonNode(m_pButtonFlashCardNode);

	this->addChild(m_pButtonManagerNode);

	m_pSettingNode = NULL;
	
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
		Sprite* pIconNotiy = Sprite::create("Footer/noitify_msg.png");
		pIconNotiy->setPosition(Point(95.0f, 110.0f));

		char sNumberNew[10];
		sprintf(sNumberNew, "%d", iWordNew);
		LabelTTF* pLabelNumber = LabelTTF::create(sNumberNew, "Arial", 25);
		pLabelNumber->setColor(ccc3(255.0f, 255.0f, 255.0f));
		pLabelNumber->setPosition(Point(22.0f, 20.0f));
		pIconNotiy->addChild(pLabelNumber);
		pFlashCardSprite->addChild(pIconNotiy);
	}

	int iTotalFlashCard = WordTable::getInstance()->getNumberWordCollected();
	char sTotalFlashCard[10];
	sprintf(sTotalFlashCard, "%d", iTotalFlashCard);
	LabelTTF* pLabelTotalFlashWord = LabelTTF::create(sTotalFlashCard, "Arial", 30);
	pLabelTotalFlashWord->setColor(ccc3(0.0f, 0.0f, 0.0f));
	pLabelTotalFlashWord->setPosition(Point(61.0f, 50.0f));
	pFlashCardSprite->addChild(pLabelTotalFlashWord);

	switch (statusButtonFlashcard)
	{
		case StatusButtonFlashcard::eNoClick :
		{
			pFlashCardSprite->setPosition(Point(593.0f, 48.0f));
			pFlashCardSprite->setScale(0.8f);
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