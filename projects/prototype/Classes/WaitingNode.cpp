#include "WaitingNode.h"

USING_NS_CC;

WaitingNode::~WaitingNode ()
{
}

WaitingNode* WaitingNode::createLayout(char* sTitle)
{
	WaitingNode* pWaitingNode = new WaitingNode();
	pWaitingNode->m_sTitle = sTitle;


	if(pWaitingNode->init())
	{
		pWaitingNode->autorelease();
		return pWaitingNode;
	}

	CC_SAFE_DELETE(pWaitingNode);
	return NULL;
}

bool WaitingNode::init()
{
	if (!CustomNode::init())
	{
		return false;
	}

	LayerColor* pBackground = LayerColor::create(ccc4(7, 25, 44, 150));
	pBackground->setContentSize(CCSizeMake(640.0f, 960.0f));
	this->addChild(pBackground);
	this->setContentSize(pBackground->getContentSize());

	Sprite* pBackgroundBoard = Sprite::create("popup/panel_popup_waiting.png");
	pBackgroundBoard->setPosition(Point(320.0f, 480.0f));
	pBackgroundBoard->setScaleY(0.7f);
	this->addChild(pBackgroundBoard);

	m_pLabelTitle = LabelBMFont::create(m_sTitle, "fonts/font_title-popup.fnt", 410.0f, TextHAlignment::CENTER);
	m_pLabelTitle->setAnchorPoint(Point(0.5f, 0.5f));
	m_pLabelTitle->setPosition(Point(320.0f, 485.0f));
	this->addChild(m_pLabelTitle);

	return true;
}

bool WaitingNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}