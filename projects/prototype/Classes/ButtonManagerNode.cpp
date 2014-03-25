#include "ButtonManagerNode.h" 
#include "SoundManager.h"

USING_NS_CC;

ButtonManagerNode::~ButtonManagerNode()
{
	
}

bool ButtonManagerNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { return this->onTouchButtonBegan(touch, event);  };
	listener->onTouchMoved = [this](Touch* touch, Event* event) { this->onTouchButtonMoved(touch, event); };
	listener->onTouchEnded = [this](Touch* touch, Event* event) { this->onTouchButtonEnded(touch, event); };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, this);
	
	m_iIndexButtonActive = -1;
	m_isClickButton = false;

	return true;
}

bool ButtonManagerNode::onTouchButtonBegan(Touch* pTouch,  Event* pEvent)
{
	m_pointClickBegin = pTouch->getLocation();

	for (int iIndex=0; iIndex<m_buttonNodes.size(); iIndex++)
	{
		Sprite* pButtonSprite;
		if(m_buttonNodes[iIndex]->getStateActive())
			pButtonSprite = m_buttonNodes[iIndex]->getButtonSpriteActive();
		else
			pButtonSprite = m_buttonNodes[iIndex]->getButtonSprite();

		CCRect *pRectButton = new CCRect(pButtonSprite->getOffsetPosition().x, 
			 pButtonSprite->getOffsetPosition().y, 
			 pButtonSprite->getTextureRect().size.width, 
			 pButtonSprite->getTextureRect().size.height);

		Point touchButton = pButtonSprite->convertToNodeSpace(m_pointClickBegin);

		if(pRectButton->containsPoint(touchButton))
		{
			m_isClickButton = true;
			auto scaleAction = ScaleTo::create(0.1f, 1.150f, 0.85f);
			pButtonSprite->runAction(scaleAction);
			m_iIndexButtonActive = iIndex;

			// play sound effect
			SoundManager::PlaySoundEffect(_SET_BUTTON_PRESS_);

			return true;
			break;
		}
	}

	return false;
	
}

void ButtonManagerNode::onTouchButtonMoved(Touch* pTouch,  Event* pEvent)
{
	
}

void ButtonManagerNode::onTouchButtonEnded(Touch* pTouch,  Event* pEvent)
{
	m_pointClickEnd = pTouch->getLocation();

	if (m_isClickButton)
	{
		Sprite* pButtonSprite;
		if(m_buttonNodes[m_iIndexButtonActive]->getStateActive())
			pButtonSprite = m_buttonNodes[m_iIndexButtonActive]->getButtonSpriteActive();
		else
			pButtonSprite = m_buttonNodes[m_iIndexButtonActive]->getButtonSprite();

		CCRect *pRectButton = new CCRect(pButtonSprite->getOffsetPosition().x, 
				 pButtonSprite->getOffsetPosition().y, 
				 pButtonSprite->getTextureRect().size.width, 
				 pButtonSprite->getTextureRect().size.height);

		Point touchButton = pButtonSprite->convertToNodeSpace(m_pointClickEnd);

		if(pRectButton->containsPoint(touchButton))
		{
			auto actionCallback = CallFunc::create(this, callfunc_selector(ButtonManagerNode::buttonCallBack));
			auto scaleAction = ScaleTo::create(0.05, 0.9f, 1.1f);
			auto scaleAction1 = ScaleTo::create(0.08, 1.1f, 0.9f);
			auto scaleAction2 = ScaleTo::create(0.1, 1.0f, 1.0f);
			pButtonSprite->runAction(Sequence::create(scaleAction, scaleAction1, scaleAction2, actionCallback, NULL));
		}
		else
		{
			auto scaleAction = ScaleTo::create(0.05, 0.9f, 1.1f);
			auto scaleAction1 = ScaleTo::create(0.08, 1.1f, 0.9f);
			auto scaleAction2 = ScaleTo::create(0.1, 1.0f, 1.0f);
			pButtonSprite->runAction(Sequence::create(scaleAction, scaleAction1, scaleAction2, NULL));
		}
	}

	m_isClickButton = false;
}

void ButtonManagerNode::buttonCallBack()
{
	ButtonNode* pButtonSprite = m_buttonNodes[m_iIndexButtonActive];
	pButtonSprite->setStateActive(!pButtonSprite->getStateActive());

	if(pButtonSprite->getCallBack())
	{
		auto callBack = pButtonSprite->getCallBack();
		callBack(pButtonSprite);
	}
}

void ButtonManagerNode::addButtonNode(ButtonNode* pButtonNode)
{
	m_buttonNodes.push_back(pButtonNode);
	this->addChild(pButtonNode);
}