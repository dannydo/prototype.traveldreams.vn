#ifndef _WAITING_NODE_H_
#define _WAITING_NODE_H_

#include "cocos2d.h"
#include "CustomNode.h"

USING_NS_CC;

class WaitingNode : public CustomNode
{
public:
	WaitingNode() {};
	virtual ~WaitingNode();
	bool init() override;
	static WaitingNode* createLayout(char* sTitle);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

	void UpdateTitle(const char* sTitle)
	{
		m_pLabelTitle->setCString(sTitle);
	}

private:
	void clickUseLocal();
	void clickUseFacebook();

private:
	char* m_sTitle;
	LabelBMFont *m_pLabelTitle;
};

#endif;