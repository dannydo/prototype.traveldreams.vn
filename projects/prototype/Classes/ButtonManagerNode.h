#ifndef _BUTTON_MANAGER_H_
#define _BUTTON_MANAGER_H_

#include "cocos2d.h"
#include "ButtonNode.h"


class ButtonManagerNode : public cocos2d::Node
{

public:
	ButtonManagerNode() {};
	virtual ~ButtonManagerNode();
	bool init() override;
	CREATE_FUNC(ButtonManagerNode);

	inline bool IsInTapMode() { return m_bIsInTapMode;}
	void AllowSwipingBackground(bool bAllowSwipingBackground);

	virtual bool onTouchButtonBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);
	virtual void onTouchButtonMoved(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);
	virtual void onTouchButtonEnded(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent);

	void addButtonNode(ButtonNode* pButtonNode);
	void removeButtonNode(ButtonNode* pButtonNode);

private:
	void buttonCallBack();

	cocos2d::Point m_pointClickBegin;
	cocos2d::Point m_pointClickEnd;
	std::vector<ButtonNode*> m_buttonNodes;

	int m_iIndexButtonActive;

	bool m_bIsInTapMode;
	bool m_bAllowSwipingBackground;
	cocos2d::EventListenerTouch*  m_pEventListener;

	bool m_isClickButton;
};

#endif;