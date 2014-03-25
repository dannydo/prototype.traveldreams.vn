#ifndef __USER_TABLE_H__
#define __USER_TABLE_H__

#include "cocos2d.h"

#define _SECONDS_FOR_NEW_LIFE_ 1800
#define _MAX_LIFE_ 5

struct UserInfo
{
	std::string sUserIdentifier;
	int iUserId;
	std::string sFacebookId;
	std::string sFacebookToken;
	std::string sFirstName;
	std::string sLastName;
	int iCurrentChapter;
	int iCurrentLevel;
	int iLife;
	int iLifeTimeRemaining;
	unsigned long ulLifeTimeBeginRemain;
};

class UserTable
{
public:
	static UserTable* getInstance();
	bool init();
	UserTable();
	static void releaseInstance();

	inline UserInfo getUserInfo() { return m_userInfo; };
	void fetchhUser();
	bool updateUser(const UserInfo& userInfo);
	bool insertUser(const UserInfo& userInfo);
	bool updateLife(const unsigned int& iLoseLife);

private:
	unsigned long getTimeLocalCurrent();

	static UserTable* m_UserTable;
	UserInfo m_userInfo;
};
#endif
