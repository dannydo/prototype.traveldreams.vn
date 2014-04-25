#ifndef _GET_MORE_LIFE_NODE_H_
#define _GET_MORE_LIFE_NODE_H_

#include "cocos2d.h"
#include "ButtonManagerNode.h" 
#include "Database\UserTable.h"


typedef enum {
	eGoToMainMenu = 0,
	eClosePopup,
} GetMoreLifeType; 


class GetMoreLifeNode : public cocos2d::Node
{
public:
	bool init() override;
	CREATE_FUNC(GetMoreLifeNode);

	void setGetMoreLifeType(GetMoreLifeType getMoreLifeType);

private:
	void clickBuy(cocos2d::Object* sender);
	void clickAskFriend(cocos2d::Object* sender);
	void clickClose(cocos2d::Object* sender);
	void updateWhenTimeChange(float dt);

	cocos2d::String formatSecondsToDiaplay(const int& iSeconds);
	unsigned long getTimeLocalCurrent();

	cocos2d::LabelBMFont* m_pLabelSecondsRemaing;
	cocos2d::LabelBMFont* m_pLabelLife;

	UserInfo m_userInfo;
	GetMoreLifeType m_GetMoreLifeType;
};

#endif;