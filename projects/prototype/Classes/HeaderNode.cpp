#include "HeaderNode.h"
#include "LifeSystemNode.h"

USING_NS_CC;

HeaderNode::~HeaderNode()
{
	
}

bool HeaderNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	Sprite* pBarTop = Sprite::create("Footer/Header.png");
	pBarTop->setAnchorPoint(Point(0.0f, 0.0f));
	pBarTop->setPosition(Point(0.0f, 0.0f));
	this->addChild(pBarTop);

	LifeSystemNode* pLifeNode = LifeSystemNode::create();
	pLifeNode->setPosition(Point(0.0f, 55.0f));
	this->addChild(pLifeNode);

	Sprite* pMessageSprite = Sprite::create("Footer/msg-icon.png");
	pMessageSprite->setPosition(Point(520.0f, 55.0f));
	this->addChild(pMessageSprite);

	Sprite* pWifiSprite = Sprite::create("Footer/wifi-icon.png");
	pWifiSprite->setPosition(Point(605.0f, 55.0f));
	this->addChild(pWifiSprite);

	this->setPosition(Point(0.0f, 872.0f));

	return true;
}