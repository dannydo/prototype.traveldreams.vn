#ifndef _POPUP_THANKS_NODE_H_
#define _POPUP_THANKS_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"

class PopupThanksNode : public CustomNode
{
public:
	PopupThanksNode() {};
	virtual ~PopupThanksNode();
	bool init() override;
	static PopupThanksNode* createLayout();

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void clickClose(cocos2d::Object* pSender);
	void clickAdvanceMode(cocos2d::Object* pSender);
};

#endif;