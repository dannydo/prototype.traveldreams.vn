#ifndef _HEADER_NODE_H_
#define _HEADER_NODE_H_

#include "cocos2d.h"

class HeaderNode : public cocos2d::Node
{

public:
	HeaderNode() {};
	virtual ~HeaderNode();
	bool init() override; 
	CREATE_FUNC(HeaderNode);

	void updateLayoutMonney(const int& iMonney);

private:
	void clickBuyGold(cocos2d::Object* sender);
	void clickMail(cocos2d::Object* sender);

	cocos2d::LabelBMFont* m_pLabelGold;
};

#endif;