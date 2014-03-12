#include "ActionExtension.h"


//************************ ResetAnchorPointToCenterAction *******************
ResetAnchorToCenterAction::ResetAnchorToCenterAction()
{
}

ResetAnchorToCenterAction* ResetAnchorToCenterAction::Create()
{
	ResetAnchorToCenterAction* pResetAnchor = new ResetAnchorToCenterAction();
	pResetAnchor->autorelease();

	return pResetAnchor;
}

ResetAnchorToCenterAction* ResetAnchorToCenterAction::clone() const
{
	return ResetAnchorToCenterAction::Create();
}

ResetAnchorToCenterAction* ResetAnchorToCenterAction::reverse() const
{
	return ResetAnchorToCenterAction::Create();
}

void ResetAnchorToCenterAction::update(float time)
{
	CC_UNUSED_PARAM(time);
	//_target->setAnchorPoint(Point(0.5f, 0.5f));

	Size size = _target->getContentSize();
	Point currentPos = _target->getPosition();
	Point subtractAnchor = Point(0.5f, 0.5f) - _target->getAnchorPoint();

	Point translate( subtractAnchor.x * size.width, subtractAnchor.y * size.height);
	_target->setPosition( currentPos + translate);

	_target->setAnchorPoint(Point(0.5f, 0.5f));
}


//************************ ResetAnchorPointToCenterAction *******************
SetAnchorAction::SetAnchorAction(Point point)
{
	m_AnchorPoint = point;
}

SetAnchorAction* SetAnchorAction::Create(Point point)
{
	SetAnchorAction* pSetAnchor = new SetAnchorAction(point);
	pSetAnchor->autorelease();
	return pSetAnchor;
}

SetAnchorAction* SetAnchorAction::clone() const
{
	SetAnchorAction* pSetAnchor = new SetAnchorAction(m_AnchorPoint);
	pSetAnchor->autorelease();
	return pSetAnchor;
}

SetAnchorAction* SetAnchorAction::reverse() const
{
	SetAnchorAction* pSetAnchor = new SetAnchorAction(m_AnchorPoint);
	pSetAnchor->autorelease();
	return pSetAnchor;
}
	
void SetAnchorAction::update(float time)
{
	CC_UNUSED_PARAM(time);
	
	Size size = _target->getContentSize();
	Point currentPos = _target->getPosition();
	Point subtractAnchor = m_AnchorPoint - _target->getAnchorPoint();

	Point translate( subtractAnchor.x * size.width, subtractAnchor.y * size.height);
	_target->setPosition( currentPos + translate);

	_target->setAnchorPoint(m_AnchorPoint);
}	