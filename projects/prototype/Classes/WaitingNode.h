#ifndef _WAITING_NODE_H_
#define _WAITING_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"

class WaitingNode : public CustomNode
{
public:
	WaitingNode() {};
	virtual ~WaitingNode();
	bool init() override;
	static WaitingNode* createLayout(char* sTitle);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void clickUseLocal();
	void clickUseFacebook();
	char* m_sTitle;
};

#endif;