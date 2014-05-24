#ifndef __LIFE_SYSTEM_H__
#define __LIFE_SYSTEM_H__

#include "cocos2d.h" 
#include "Database\UserTable.h"

class LifeSystemNode : public cocos2d::Node
{
public:
	LifeSystemNode();
	virtual ~LifeSystemNode();
	bool init() override;
	CREATE_FUNC(LifeSystemNode);
	void update(float dt) override;

private:
	cocos2d::String formatSecondsToDiaplay(const int& iSeconds);
	void clickGetLife(cocos2d::Object* sender);

	cocos2d::LabelBMFont* m_pLabelSecondsRemaing;
	cocos2d::LabelBMFont* m_pLabelLife;
	cocos2d::Sprite* m_pBackgroundClock;
	cocos2d::Sprite* m_pBackgroundLife;

	UserInfo m_userInfo;
};

#endif;