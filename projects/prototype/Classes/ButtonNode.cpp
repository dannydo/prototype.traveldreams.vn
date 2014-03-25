#include "ButtonNode.h"

USING_NS_CC;

ButtonNode::~ButtonNode()
{
	
}

ButtonNode* ButtonNode::createButtonSprite(Sprite* pSpriteButton, const buttonNodeCallback& callBack)
{
	ButtonNode* pButtonNode = new ButtonNode();
	pButtonNode->m_pButtonSprite = pSpriteButton;
	pButtonNode->m_callBack = callBack;
	pButtonNode->m_bHaveStateButton = false;

	if(pButtonNode->init())
	{
		pButtonNode->autorelease();
		return pButtonNode;
	}

	CC_SAFE_DELETE(pButtonNode);
	return NULL;
}


ButtonNode* ButtonNode::createButtonSprite(Sprite* pSpriteButton, cocos2d::Sprite *pSpriteButtonActive, const buttonNodeCallback& callBack)
{
	ButtonNode* pButtonNode = new ButtonNode();
	pButtonNode->m_pButtonSprite = pSpriteButton;
	pButtonNode->m_pButtonSpriteActive = pSpriteButtonActive;
	pButtonNode->m_callBack = callBack;
	pButtonNode->m_bHaveStateButton = true;
	
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

	m_bIsActive = false;

	this->addChild(m_pButtonSprite);

	if (m_bHaveStateButton)
	{
		this->addChild(m_pButtonSpriteActive);
		m_pButtonSpriteActive->setVisible(false);
	}

	return true;
}

void ButtonNode::setStateActive(const bool& isActive)
{
	if (m_bHaveStateButton)
	{
		m_bIsActive = isActive;
		if (isActive)
		{
			m_pButtonSpriteActive->setVisible(true);
			m_pButtonSprite->setVisible(false);
		}
		else
		{
			m_pButtonSpriteActive->setVisible(false);
			m_pButtonSprite->setVisible(true);
		}
	}

}