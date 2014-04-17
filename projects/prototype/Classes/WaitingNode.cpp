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

	LayerColor* pBackground = LayerColor::create(ccc4(0, 0, 0, 255), 400.0f, 80.0f);
	pBackground->setPosition(-pBackground->getContentSize().width/2.0f, -pBackground->getContentSize().height/2.0f);
	pBackground->setAnchorPoint(Point(0.5f, 0.5f));
	this->addChild(pBackground);

	LabelTTF* pLabelTitle = LabelTTF::create(m_sTitle, "Arial", 30);
	this->addChild(pLabelTitle);

	return true;
}

bool WaitingNode::onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent)
{
	return true;
}