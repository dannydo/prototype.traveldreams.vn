#ifndef _FOOTER_NODE_H_
#define _FOOTER_NODE_H_

#include "cocos2d.h"
#include "SettingMenuNode.h"	   
#include "ButtonManagerNode.h"


class FooterNode : public cocos2d::Node
{

public:
	FooterNode() {};
	virtual ~FooterNode();
	bool init() override; 
	CREATE_FUNC(FooterNode);

	void disableButtonIntroAndFlashCard();
	inline SettingMenuNode* getSettingNode() { return m_pSettingNode; };

private:
	void openSettingMenu(cocos2d::Object* sender);
	void openFlashCardCollection(cocos2d::Object* sender);
	void openIntroduction(cocos2d::Object* sender);

	SettingMenuNode* m_pSettingNode	;

	ButtonNode* m_pButtonIntroductionNode;
	ButtonNode* m_pButtonFlashCardNode;
	ButtonNode* m_pButtonSettingNode;

};

#endif;