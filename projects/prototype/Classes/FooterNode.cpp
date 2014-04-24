#include "FooterNode.h"
#include "IntroductionLayer.h"
#include "FlashCardCollection.h"

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

	Sprite* pBarBottom = Sprite::create("Footer/Footer.png");
	pBarBottom->setAnchorPoint(Point(0.0f, 0.0f));
	pBarBottom->setPosition(Point(0.0f, 0.0f));
	this->addChild(pBarBottom);

	Sprite* pSettingSprite = Sprite::create("Footer/btn_setting.png");
	Sprite* pSettingSpriteActive = Sprite::create("Footer/btn_setting.png");
	m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, pSettingSpriteActive, CC_CALLBACK_1(FooterNode::openSettingMenu, this));
	m_pButtonSettingNode->setPosition(Point(36.0f, 33.0f));

	Sprite* pFlashCardSprite = Sprite::create("Footer/btn_flashcard.png");
	m_pButtonFlashCardNode = ButtonNode::createButtonSprite(pFlashCardSprite, CC_CALLBACK_1(FooterNode::openFlashCardCollection, this));
	m_pButtonFlashCardNode->setPosition(Point(609.0f, 33.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(m_pButtonSettingNode);
	pButtonManagerNode->addButtonNode(m_pButtonFlashCardNode);

	this->addChild(pButtonManagerNode);

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

void FooterNode::disableButtonIntroAndFlashCard()
{
	m_pButtonFlashCardNode->setVisible(false);
}