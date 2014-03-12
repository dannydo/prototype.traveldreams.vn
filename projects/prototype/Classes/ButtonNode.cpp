#include "ButtonNode.h"

USING_NS_CC;

ButtonNode::~ButtonNode()
{
	
}

ButtonNode* ButtonNode::createButtonSprite(Sprite* pSpriteButton, const buttonNodeCallback& callBack)
{
	ButtonNode* pButtonNode = new ButtonNode();
	pButtonNode->m_ButtonSprite = pSpriteButton;
	pButtonNode->m_callBack = callBack;

	if(pButtonNode->init())
	{
		pButtonNode->autorelease();
		return pButtonNode;
	}

	CC_SAFE_DELETE(pButtonNode);
	return NULL;
}

bool ButtonNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	this->addChild(m_ButtonSprite);

	return true;
}