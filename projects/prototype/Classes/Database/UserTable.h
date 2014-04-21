#ifndef __USER_TABLE_H__
#define __USER_TABLE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#define _SECONDS_FOR_NEW_LIFE_ 60
#define _MAX_LIFE_ 5

struct UserInfo
{
	std::string sUserIdentifier;
	std::string sFacebookId;
	std::string sFacebookToken;
	std::string sFirstName;
	std::string sLastName;
	std::string sCurrentChapterId;
	int iCurrentLevel;
	int iLife;
	int iLifeTimeRemaining;
	unsigned long ulLifeTimeBeginRemain;
	int iMonney;
	int iVersion;
	std::string sUserToken;
};

class UserTable
{
public:
	static UserTable* getInstance();
	bool init();
	UserTable();
	static void releaseInstance();

	inline UserInfo& getUserInfo() { return m_userInfo; };
	
	bool updateUser(const UserInfo& userInfo);
	bool updateLife(const unsigned int& iLoseLife);
	void refreshUserInfo();

	std::string syncGetUser();
	bool updateDataSyncUser(cs::JsonDictionary* pJsonSync, const int& iVersion);

private:
	void fetchhUser();
	unsigned long getTimeLocalCurrent();

	static UserTable* m_UserTable;
	UserInfo m_userInfo;
};
#endif
