#ifndef __SYSTEM_EVENT_HANDLE_H__
#define __SYSTEM_EVENT_HANDLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class SystemEventHandle
{
public:
	static SystemEventHandle* getInstance();
	SystemEventHandle();
	bool init();
	static void releaseInstance();

	void onStartSyncGame(const bool& bIsShowSyncError);
	void onGameSyncCompleted(const bool& bResult);

	void onStartConnectFacebook();
	void onLoginFacebookResult(const bool& bResult);
	void onCheckUserFacebookResult(cs::JsonDictionary* pJsonDict, std::string sKey);

private:

	static SystemEventHandle* m_SystemEventHandle;
	bool m_bIsConnectFacebook;
	bool m_bIsShowSyncError;
};
#endif