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

	m_pEventListener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	m_pEventListener->setSwallowTouches(true);
	m_pEventListener->onTouchBegan = [this](Touch* touch, Event* event) { return this->onTouchButtonBegan(touch, event);  };
	m_pEventListener->onTouchMoved = [this](Touch* touch, Event* event) { this->onTouchButtonMoved(touch, event); };
	m_pEventListener->onTouchEnded = [this](Touch* touch, Event* event) { this->onTouchButtonEnded(touch, event); };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(m_pEventListener, this);
	
	m_iIndexButtonActive = -1;
	m_isClickButton = false;

	m_bIsInTapMode = false;
	m_bAllowSwipingBackground = false;
	m_isFinishProcessButton = true;
		 
	return true;
}

bool ButtonManagerNode::onTouchButtonBegan(Touch* pTouch,  Event* pEvent)
{
	if (m_isFinishProcessButton == false)
		return false;

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
			m_isFinishProcessButton = false;
			auto scaleAction = ScaleTo::create(0.05f, 0.9f);
			auto scaleAction1 = ScaleTo::create(0.08f, 0.93f);
			auto scaleAction2 = ScaleTo::create(0.1f, 0.9f);

			pButtonSprite->runAction(Sequence::create(scaleAction, scaleAction1, scaleAction2, NULL));
			m_iIndexButtonActive = iIndex;

			// play sound effect
			SoundManager::PlaySoundEffect(_SET_BUTTON_PRESS_);

			m_bIsInTapMode = true;
			return true;
		}
	}

	return false;
	
}

void ButtonManagerNode::onTouchButtonMoved(Touch* pTouch,  Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();
	if (fabsf(m_pointClickBegin.x - touchPosition.x) > 30 || fabsf(m_pointClickBegin.y - touchPosition.y) > 30)
	{
		m_bIsInTapMode = false;
		m_isFinishProcessButton = true;
	}
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

		if(pRectButton->containsPoint(touchButton) && m_bIsInTapMode)
		{
			auto actionCallback = CallFunc::create(this, callfunc_selector(ButtonManagerNode::buttonCallBack));
			auto scaleAction = ScaleTo::create(0.05, 1.05f);
			auto scaleAction1 = ScaleTo::create(0.08, 0.98f);
			auto scaleAction2 = ScaleTo::create(0.1, 1.0f);
			pButtonSprite->runAction(Sequence::create(scaleAction, scaleAction1, scaleAction2, actionCallback, NULL));
		}
		else
		{
			auto scaleAction = ScaleTo::create(0.05, 1.05f);
			auto scaleAction1 = ScaleTo::create(0.08, 0.98f);
			auto scaleAction2 = ScaleTo::create(0.1, 1.0f);
			pButtonSprite->runAction(Sequence::create(scaleAction, scaleAction1, scaleAction2, NULL));
		}
	}

	m_bIsInTapMode = false;
	m_isClickButton = false;
}

void ButtonManagerNode::buttonCallBack()
{
	m_isFinishProcessButton = true;

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

void ButtonManagerNode::removeButtonNode(ButtonNode* pButtonNode)
{
	int iIndex=-1;
	for (iIndex=0; iIndex<m_buttonNodes.size(); iIndex++)
	{
		if(m_buttonNodes[iIndex] == pButtonNode)
			break;
	}

	if (iIndex >= 0 && iIndex < m_buttonNodes.size())
	{
		for (int i=iIndex; i<m_buttonNodes.size()-1; i++)
		{
			m_buttonNodes[i] = m_buttonNodes[i+1];
		}

		m_buttonNodes.pop_back();
		this->removeChild(pButtonNode);
	}
}

void ButtonManagerNode::AllowSwipingBackground(bool bAllowSwipingBackground)
{ 
	m_bAllowSwipingBackground = bAllowSwipingBackground;
	m_pEventListener->setSwallowTouches(!m_bAllowSwipingBackground);
}