#ifndef _CONNECT_FACEBOOK_CONFIRM_H_
#define _CONNECT_FACEBOOK_CONFIRM_H_

#include "cocos2d.h"
#include "CustomNode.h"
#include "Database\UserTable.h"

class ConnectFacebookConfirm : public CustomNode
{
public:
	ConnectFacebookConfirm() {};
	virtual ~ConnectFacebookConfirm();
	bool init() override;
	static ConnectFacebookConfirm* createLayout(const UserInfo& userInfo);

	bool onTouchCustomNodeBegan(cocos2d::Touch* pTouch,  cocos2d::Event* pEvent) override;

private:
	void clickUseLocal();
	void clickUseFacebook();
	void runProcess();
	void finishProcess();

	UserInfo m_UserInfo;
};

#endif;