#ifndef _FOOTER_NODE_H_
#define _FOOTER_NODE_H_

#include "cocos2d.h"
#include "SettingMenuNode.h"	   
#include "ButtonManagerNode.h"

typedef enum {
	eNoClick = 0,
    eClickShowFlashcardCollection,
} StatusButtonFlashcard;

class FooterNode : public cocos2d::Node
{

public:
	FooterNode() {};
	virtual ~FooterNode();
	bool init() override; 
	CREATE_FUNC(FooterNode);

	inline SettingMenuNode* getSettingNode() { return m_pSettingNode; };

	void removeButtonFlashcard();
	void removeBackground();
	void changeStatusButtonFlashcard(StatusButtonFlashcard statusButtonFlashcard);

private:
	void openSettingMenu(cocos2d::Object* sender);
	void openFlashCardCollection(cocos2d::Object* sender);

	cocos2d::Sprite* m_pBackground;
	SettingMenuNode* m_pSettingNode	;

	ButtonNode* m_pButtonFlashCardNode;
	ButtonNode* m_pButtonSettingNode;

	ButtonManagerNode* m_pButtonManagerNode;

};

#endif;