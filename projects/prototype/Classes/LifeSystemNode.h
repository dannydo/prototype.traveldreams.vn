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


private:
	void updateWhenTimeChange(float dt);
	unsigned long getTimeLocalCurrent();
	cocos2d::String formatSecondsToDiaplay(const int& iSeconds);

	cocos2d::LabelTTF* m_pLabelSecondsRemaing;
	cocos2d::LabelTTF* m_pLabelLife;

	UserInfo m_userInfo;
};

#endif;