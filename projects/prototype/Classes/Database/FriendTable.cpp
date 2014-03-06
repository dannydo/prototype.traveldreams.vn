#include "FriendTable.h"
#include "InitDatabase.h"

USING_NS_CC; 

FriendTable* FriendTable::m_FriendTable = NULL;

FriendTable::FriendTable()
{

}

void FriendTable::releaseInstance()
{
	if (m_FriendTable == NULL)
	{
		delete m_FriendTable;
	}
	m_FriendTable = NULL;	
}


FriendTable* FriendTable::getInstance()
{
	if (m_FriendTable == NULL) {
		m_FriendTable = new FriendTable();
		m_FriendTable->init();
	}

	return m_FriendTable;
}

bool FriendTable::init()
{
	fetchAllFriend();
	return true;
}

void FriendTable::fetchAllFriend()
{
	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from Friends", &re, &nRow, &nColumn,NULL);
	CCLOG("row is %d,column is %d", nRow, nColumn);

	for (int iRow=1; iRow<=nRow; iRow++)
	{
		FriendInfo friendInfo;
		friendInfo.iFriendId = int(strtod(re[iRow*nColumn+0], NULL));
		friendInfo.sFacebookId = re[iRow*nColumn+1];
		friendInfo.sFirstName = re[iRow*nColumn+2];
		friendInfo.sLastName = re[iRow*nColumn+3];
		friendInfo.iCurrentChapter = int(strtod(re[iRow*nColumn+4], NULL));
		friendInfo.iCurrentLevel = int(strtod(re[iRow*nColumn+5], NULL));

		m_Friends.push_back(friendInfo);
	}

	sqlite3_free_table(re);
}