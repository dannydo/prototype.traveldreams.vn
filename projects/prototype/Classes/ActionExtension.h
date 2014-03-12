#ifndef _ACTION_EXTENSION_H_
#define _ACTION_EXTENSION_H_

#include "cocos2d.h"

using namespace cocos2d;

class ScaleAtBottonTo: public ActionInterval
{
public:

protected:
	float m_fScaleX;
	float m_fScaleY;
	float m_fStartScaleX;
	float m_fStartScaleY;
	float m_fEndScaleX;
	float m_fEndScaleY;

	float m_fDeltaX;
	float m_fDeltaY;

    float _scaleY;
    float _startScaleX;
    float _startScaleY;
    float _endScaleX;
    float _endScaleY;
    float _deltaX;
    float _deltaY;
};

class ResetAnchorToCenterAction : public ActionInstant
{
public:
	ResetAnchorToCenterAction();

	static ResetAnchorToCenterAction* Create();

	virtual ResetAnchorToCenterAction* clone() const override;
	virtual ResetAnchorToCenterAction* reverse() const override;
	
    virtual void update(float time) override;
};


class SetAnchorAction : public ActionInstant
{
public:
	SetAnchorAction(Point point);

	static SetAnchorAction* Create(Point point);

	virtual SetAnchorAction* clone() const override;
	virtual SetAnchorAction* reverse() const override;
	
    virtual void update(float time) override;
protected:
	Point m_AnchorPoint;
};

#endif  _ACTION_EXTENSION_H_