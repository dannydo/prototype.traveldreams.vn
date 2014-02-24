#ifndef __SETTING_MENU_H__
#define __SETTING_MENU_H__

#include "cocos2d.h" 
#include "Database\UserTable.h"

class SettingMenuNode : public cocos2d::Node
{
public:
	SettingMenuNode();
	virtual ~SettingMenuNode();
	bool init() override;
	CREATE_FUNC(SettingMenuNode);


private:
	
};

#endif;