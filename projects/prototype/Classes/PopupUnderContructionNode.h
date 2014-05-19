#ifndef _POPUP_CONTRUCTION_NODE_H_
#define _POPUP_CONTRUCTION_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"

class PopupUnderContructionNode : public CustomNode
{
public:
	PopupUnderContructionNode() {};
	virtual ~PopupUnderContructionNode();
	bool init() override;
	static PopupUnderContructionNode* createLayout();

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void clickOk(cocos2d::Object* pSender);
};

#endif;