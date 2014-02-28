#ifndef __FRIEND_TABLE_H__
#define __FRIEND_TABLE_H__

#include "cocos2d.h"

struct FriendInfo
{
	int iFriendId;
	std::string sFacebookId;
	std::string sFirstName;
	std::string sLastName;
	int iCurrentChapter;
	int iCurrentLevel;
};

class FriendTable
{
public:
	static FriendTable* getInstance();
	bool init();
	inline std::vector<FriendInfo> getFriendsInfo() { return m_Friends; };
	void fetchAllFriend();

private:
	FriendTable();
	virtual ~FriendTable();

	static FriendTable* m_FriendTable;
	std::vector<FriendInfo> m_Friends;
};
#endif