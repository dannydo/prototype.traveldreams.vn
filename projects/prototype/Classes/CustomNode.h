#ifndef _CUSTOM_NODE_H_
#define _CUSTOM_NODE_H_

#include "cocos2d.h"

typedef enum {
	_CUSTOM_NODE_TAP_ = 0,
	_CUSTOM_NODE_SWIPE_,
} CustomNodeClickMode;

class CustomNode : public cocos2d::Node
{
public:
	bool init() override;

	CREATE_FUNC(CustomNode);

	inline CustomNodeClickMode getClickMode() { return m_clickMode; };

	virtual void onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);
	virtual void onTouchCustomNodeMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent); 
	virtual void onTouchCustomNodeEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);

private:
	CustomNodeClickMode m_clickMode;
	cocos2d::Point m_pointClickBegin;
};

#endif;