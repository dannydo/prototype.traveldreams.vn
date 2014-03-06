#include "CustomNode.h"

USING_NS_CC;

bool CustomNode::init()
{
	if(!Node::init())
	{
		return false;
	}

	m_clickMode = CustomNodeClickMode::_CUSTOM_NODE_TAP_;

	auto listener = EventListenerTouch::create(Touch::DispatchMode::ONE_BY_ONE);
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event* event) { this->onTouchCustomNodeBegan(touch, event); return true; };
	listener->onTouchMoved = [this](Touch* touch, Event* event) { this->onTouchCustomNodeMoved(touch, event); };
	listener->onTouchEnded = [this](Touch* touch, Event* event) { this->onTouchCustomNodeEnded(touch, event); };
	EventDispatcher::getInstance()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void CustomNode::onTouchCustomNodeBegan(Touch* pTouch,  Event* pEvent)
{
	m_pointClickBegin = pTouch->getLocation();
}

void CustomNode::onTouchCustomNodeMoved(Touch* pTouch,  Event* pEvent)
{
	Point touchPosition = pTouch->getLocation();
	if((fabs(touchPosition.x) > fabs(m_pointClickBegin.x) + 10) || (fabs(touchPosition.y) > fabs(m_pointClickBegin.y) + 10))
	{
		m_clickMode = CustomNodeClickMode::_CUSTOM_NODE_SWIPE_;
	}
}  

void CustomNode::onTouchCustomNodeEnded(Touch* pTouch,  Event* pEvent)
{
	pEvent->stopPropagation();
}