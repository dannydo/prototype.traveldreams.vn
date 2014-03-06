#include "UserTable.h"
#include "InitDatabase.h"

USING_NS_CC; 

UserTable* UserTable::m_UserTable = NULL;

UserTable::UserTable()
{

}

void UserTable::releaseInstance()
{
	if (m_UserTable == NULL)
	{
		delete m_UserTable;
	}
	m_UserTable = NULL;	
}

UserTable* UserTable::getInstance()
{
	if (m_UserTable == NULL) {
		m_UserTable = new UserTable();
		m_UserTable->init();
	}

	return m_UserTable;
}

bool UserTable::init()
{
	fetchhUser();
	return true;
}

void UserTable::fetchhUser()
{
	char **re;
	int nRow, nColumn;
		
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), "select * from Users limit 1", &re, &nRow, &nColumn,NULL);
	CCLOG("row is %d,column is %d", nRow, nColumn);

	if (nRow > 0)
	{
		m_userInfo.sUserIdentifier = re[nColumn+0];
		m_userInfo.iUserId = int(strtod(re[nColumn+1], NULL));
		m_userInfo.sFacebookId = re[nColumn+2];
		m_userInfo.sFacebookToken = re[nColumn+3];
		m_userInfo.sFirstName = re[nColumn+4];
		m_userInfo.sLastName = re[nColumn+5];
		m_userInfo.iCurrentChapter = int(strtod(re[nColumn+6], NULL));
		m_userInfo.iCurrentLevel = int(strtod(re[nColumn+7], NULL));
		m_userInfo.iLife = int(strtod(re[nColumn+8], NULL));
		m_userInfo.iLifeTimeRemaining = int(strtod(re[nColumn+9], NULL));
		m_userInfo.ulLifeTimeBeginRemain = int(strtod(re[nColumn+10], NULL));
	}

	sqlite3_free_table(re);
}

bool UserTable::updateUser(const UserInfo& userInfo)
{
	String sql = "update Users Set";
	sql.appendWithFormat(" UserId=%d,", userInfo.iUserId);
	sql.appendWithFormat(" FacebookId='%s',", userInfo.sFacebookId.c_str());
	sql.appendWithFormat(" FacebookToken='%s',", userInfo.sFacebookToken.c_str());
	sql.appendWithFormat(" FirstName='%s',", userInfo.sFirstName.c_str());
	sql.appendWithFormat(" LastName='%s',", userInfo.sLastName.c_str());
	sql.appendWithFormat(" CurrentChapter=%d,", userInfo.iCurrentChapter);
	sql.appendWithFormat(" CurrentLevel=%d,", userInfo.iCurrentLevel);
	sql.appendWithFormat(" Life=%d,", userInfo.iLife);
	sql.appendWithFormat(" LifeTimeRemaining=%d,", userInfo.iLifeTimeRemaining);
	sql.appendWithFormat(" LifeTimeBeginRemain=%u", userInfo.ulLifeTimeBeginRemain);
	sql.appendWithFormat(" where UserIdentifier='%s'", m_userInfo.sUserIdentifier.c_str());
	CCLOG("%s", sql.getCString());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	m_userInfo = userInfo;

	return true;
}

bool UserTable::insertUser(const UserInfo& userInfo)
{
	String sql = "insert into Users values(";
	sql.appendWithFormat("'%s',", userInfo.sUserIdentifier.c_str());
	sql.appendWithFormat("%d,", userInfo.iUserId);
	sql.appendWithFormat("'%s',", userInfo.sFacebookId.c_str());
	sql.appendWithFormat("'%s',", userInfo.sFacebookToken.c_str());
	sql.appendWithFormat("'%s',", userInfo.sFirstName.c_str());
	sql.appendWithFormat("'%s',", userInfo.sLastName.c_str());
	sql.appendWithFormat("%d,", userInfo.iCurrentChapter);
	sql.appendWithFormat("%d,", userInfo.iCurrentLevel);
	sql.appendWithFormat("%d,", userInfo.iLife);
	sql.appendWithFormat("%d,", userInfo.iLifeTimeRemaining);
	sql.appendWithFormat("%u)", userInfo.ulLifeTimeBeginRemain);
	CCLOG("%s", sql.getCString());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;

	m_userInfo = userInfo;

	return true;
}

bool UserTable::updateLife(const unsigned int& iLoseLife)
{
	unsigned long m_lCurentTime = getTimeLocalCurrent();
	
	if (m_userInfo.iLife == 5)
	{
		m_userInfo.iLife -= iLoseLife;
		if (iLoseLife > 0)
			m_userInfo.iLifeTimeRemaining = _SECONDS_FOR_NEW_LIFE_;
		else
			m_userInfo.iLifeTimeRemaining = 0;
	}
	else
	{
		unsigned long lSecondsRmain  = m_lCurentTime - m_userInfo.ulLifeTimeBeginRemain + _SECONDS_FOR_NEW_LIFE_ - m_userInfo.iLifeTimeRemaining;
		if (lSecondsRmain < 0)
		{
			m_userInfo.iLife += lSecondsRmain/_SECONDS_FOR_NEW_LIFE_;
			if (m_userInfo.iLife < 0)
				m_userInfo.iLife = 0;
			
			m_userInfo.iLifeTimeRemaining = _SECONDS_FOR_NEW_LIFE_;
		}
		else
		{
			if(lSecondsRmain >= _SECONDS_FOR_NEW_LIFE_)
			{
				m_userInfo.iLife += lSecondsRmain/_SECONDS_FOR_NEW_LIFE_;
				if (m_userInfo.iLife >= _MAX_LIFE_)
				{
					m_userInfo.iLife = _MAX_LIFE_;
					m_userInfo.iLifeTimeRemaining = 0;

					if (iLoseLife > 0)
						m_userInfo.iLifeTimeRemaining = _SECONDS_FOR_NEW_LIFE_;

					m_userInfo.iLife -= iLoseLife;
				}
				else
				{
					m_userInfo.iLifeTimeRemaining = _SECONDS_FOR_NEW_LIFE_ - lSecondsRmain%_SECONDS_FOR_NEW_LIFE_;
					m_userInfo.iLife -= iLoseLife;
				}
			}
			else
			{
				m_userInfo.iLifeTimeRemaining = _SECONDS_FOR_NEW_LIFE_ - lSecondsRmain;
				m_userInfo.iLife -= iLoseLife;
			}
		}
	}

	m_userInfo.ulLifeTimeBeginRemain = m_lCurentTime;
	String sql = "update Users Set";
	sql.appendWithFormat(" Life=%d,", m_userInfo.iLife);
	sql.appendWithFormat(" LifeTimeRemaining=%d,", m_userInfo.iLifeTimeRemaining);
	sql.appendWithFormat(" LifeTimeBeginRemain=%u", m_userInfo.ulLifeTimeBeginRemain);
	sql.appendWithFormat(" where UserIdentifier='%s'", m_userInfo.sUserIdentifier.c_str());
	CCLOG("%s", sql.getCString());

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;
	
	return true;
}

unsigned long UserTable::getTimeLocalCurrent()
{
	timeval now;
	gettimeofday(&now, NULL);
	unsigned long iCurrentTime = now.tv_sec + now.tv_usec/1000000 ; //seconds
	return iCurrentTime;
}