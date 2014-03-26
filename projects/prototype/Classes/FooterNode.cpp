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
	Sprite* pSettingSpriteActive = Sprite::create("Footer/btn-back-menu.png");
	m_pButtonSettingNode = ButtonNode::createButtonSprite(pSettingSprite, pSettingSpriteActive, CC_CALLBACK_1(FooterNode::openSettingMenu, this));
	m_pButtonSettingNode->setPosition(Point(50.0f, 50.0f));

	Sprite* pFlashCardSprite = Sprite::create("Footer/btn_flashcard.png");
	m_pButtonFlashCardNode = ButtonNode::createButtonSprite(pFlashCardSprite, CC_CALLBACK_1(FooterNode::openFlashCardCollection, this));
	m_pButtonFlashCardNode->setPosition(Point(585.0f, 50.0f));

	Sprite* pIntroductionSprite = Sprite::create("Footer/info_icon.png");
	m_pButtonIntroductionNode = ButtonNode::createButtonSprite(pIntroductionSprite, CC_CALLBACK_1(FooterNode::openIntroduction, this));
	m_pButtonIntroductionNode->setPosition(Point(280.0f, 25.0f));

	ButtonManagerNode* pButtonManagerNode = ButtonManagerNode::create();
	pButtonManagerNode->addButtonNode(m_pButtonSettingNode);
	pButtonManagerNode->addButtonNode(m_pButtonFlashCardNode);
	//pButtonManagerNode->addButtonNode(m_pButtonIntroductionNode);

	this->addChild(pButtonManagerNode);
	this->setZOrder(10);

	m_pSettingNode = NULL;
	
	return true;
}

void FooterNode::openIntroduction(Object* sender)
{
	if (m_pButtonIntroductionNode->isVisible())
	{
		IntroductionScene* pIntroduction = IntroductionScene::create();
		Director::getInstance()->replaceScene(pIntroduction);
	}
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
		m_pSettingNode->setPosition(Point(-524.0f, 0.0f));
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
	m_pButtonIntroductionNode->setVisible(false);
}