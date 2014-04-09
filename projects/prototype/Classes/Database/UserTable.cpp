#include "UserTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"

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

	if (nRow > 0)
	{
		m_userInfo.sUserIdentifier = re[nColumn+0];
		m_userInfo.sFacebookId = re[nColumn+1];
		m_userInfo.sFacebookToken = re[nColumn+2];
		m_userInfo.sFirstName = re[nColumn+3];
		m_userInfo.sLastName = re[nColumn+4];
		m_userInfo.sCurrentChapterId = re[nColumn+5];
		m_userInfo.iCurrentLevel = int(strtod(re[nColumn+6], NULL));
		m_userInfo.iLife = int(strtod(re[nColumn+7], NULL));
		m_userInfo.iLifeTimeRemaining = int(strtod(re[nColumn+8], NULL));
		m_userInfo.ulLifeTimeBeginRemain = unsigned long(strtod(re[nColumn+9], NULL));
		m_userInfo.iMonney = int(strtod(re[nColumn+10], NULL));
		m_userInfo.iVersion = int(strtod(re[nColumn+11], NULL));
	}

	sqlite3_free_table(re);
}

bool UserTable::updateUser(const UserInfo& userInfo)
{
	String sql = "update Users Set";
	sql.appendWithFormat(" FacebookId='%s',", userInfo.sFacebookId.c_str());
	sql.appendWithFormat(" FacebookToken='%s',", userInfo.sFacebookToken.c_str());
	sql.appendWithFormat(" FirstName='%s',", userInfo.sFirstName.c_str());
	sql.appendWithFormat(" LastName='%s',", userInfo.sLastName.c_str());
	sql.appendWithFormat(" CurrentChapter='%s',", userInfo.sCurrentChapterId.c_str());
	sql.appendWithFormat(" CurrentLevel=%d,", userInfo.iCurrentLevel);
	sql.appendWithFormat(" Life=%d,", userInfo.iLife);
	sql.appendWithFormat(" LifeTimeRemaining=%d,", userInfo.iLifeTimeRemaining);
	sql.appendWithFormat(" LifeTimeBeginRemain=%u,", userInfo.ulLifeTimeBeginRemain);
	sql.appendWithFormat(" Monney=%d,", userInfo.iMonney);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where UserIdentifier='%s'", m_userInfo.sUserIdentifier.c_str());

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
	sql.appendWithFormat(" LifeTimeBeginRemain=%u,", m_userInfo.ulLifeTimeBeginRemain);
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionId + 1);
	sql.appendWithFormat(" where UserIdentifier='%s'", m_userInfo.sUserIdentifier.c_str());

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

std::string UserTable::syncGetUser()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Users where Version>";
	sql.appendWithFormat("%d ", VersionTable::getInstance()->getVersionInfo().iVersionSync);
	sql.append("limit 1");
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"User\":{";
	if (nRow > 0)
	{
		sJsonData.appendWithFormat("\"UserIdentifier\":\"%s\",", re[nColumn+0]);
		sJsonData.appendWithFormat("\"FacebookId\":\"%s\",", re[nColumn+1]);
		sJsonData.appendWithFormat("\"FacebookToken\":\"%s\",", re[nColumn+2]);
		sJsonData.appendWithFormat("\"FirstName\":\"%s\",", re[nColumn+3]);
		sJsonData.appendWithFormat("\"LastName\":\"%s\",", re[nColumn+4]);
		sJsonData.appendWithFormat("\"CurrentChapterId\":\"%s\",", re[nColumn+5]);
		sJsonData.appendWithFormat("\"CurrentLevel\":%s,", re[nColumn+6]);
		sJsonData.appendWithFormat("\"Life\":%s,", re[nColumn+7]);
		sJsonData.appendWithFormat("\"LifeTimeRemaining\":%s,", re[nColumn+8]);
		sJsonData.appendWithFormat("\"lLifeTimeBeginRemain\":%s,", re[nColumn+9]);
		sJsonData.appendWithFormat("\"Monney\":%s,", re[nColumn+10]);
		sJsonData.appendWithFormat("\"Version\":%s", re[nColumn+11]);
	}
	sJsonData.append("}");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}