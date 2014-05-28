#include "UserTable.h"
#include "InitDatabase.h"
#include "VersionTable.h"
#include "FunctionCommon.h"
#include "ChapterTable.h"
#include "LevelTable.h"
#include "GameConfigManager.h"

USING_NS_CC; 
USING_NS_CC_EXT;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "Social\FacebookManager.h"
using namespace cocos2d::plugin;
#endif

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
		m_userInfo.iCurrentLevel = int(strtod(re[nColumn+6], 0));
		m_userInfo.iLife = int(strtod(re[nColumn+7], 0));
		m_userInfo.iLifeTimeRemaining = int(strtod(re[nColumn+8], 0));
		m_userInfo.ulLifeTimeBeginRemain = long(strtod(re[nColumn+9], 0));
		m_userInfo.iMonney = int(strtod(re[nColumn+10], 0));
		m_userInfo.iVersion = int(strtod(re[nColumn+11], 0));
		m_userInfo.sUserToken = re[nColumn+12];
		m_userInfo.sDeviceId = re[nColumn+13];
	}

	sqlite3_free_table(re);
}

void UserTable::refreshUserInfo()
{
	this->fetchhUser();
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
	sql.appendWithFormat(" Version=%d,", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);
	sql.appendWithFormat(" UserToken='%s',", userInfo.sUserToken.c_str());
	sql.appendWithFormat(" UserIdentifier='%s'", userInfo.sUserIdentifier.c_str());

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
	sql.appendWithFormat(" Version=%d", VersionTable::getInstance()->getVersionInfo().iVersionSync + 1);

	int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		return false;
	
	return true;
}

std::string UserTable::syncGetUser()
{
	char **re;
	int nRow, nColumn;
		
	String sql = "select * from Users limit 1";
	sqlite3_get_table(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), &re, &nRow, &nColumn,NULL);

	String sJsonData = "\"User\":{";
	if (nRow > 0)
	{
		std::string sUserIdentifier = re[nColumn+0];
		if (sUserIdentifier != "ohmyword")
			sJsonData.appendWithFormat("\"UserId\":\"%s\",", re[nColumn+0]);
		else
			sJsonData.appendWithFormat("\"UserId\":\"%s\",", "");

		sJsonData.appendWithFormat("\"FacebookId\":\"%s\",", re[nColumn+1]);
		sJsonData.appendWithFormat("\"FacebookToken\":\"%s\",", re[nColumn+2]);
		sJsonData.appendWithFormat("\"FirstName\":\"%s\",", re[nColumn+3]);
		sJsonData.appendWithFormat("\"LastName\":\"%s\",", re[nColumn+4]);
		sJsonData.appendWithFormat("\"CurrentChapter\":\"%s\",", re[nColumn+5]);
		sJsonData.appendWithFormat("\"CurrentLevel\":%s,", re[nColumn+6]);
		sJsonData.appendWithFormat("\"Life\":%s,", re[nColumn+7]);
		sJsonData.appendWithFormat("\"LifeTimeRemaining\":%s,", re[nColumn+8]);
		sJsonData.appendWithFormat("\"lLifeTimeBeginRemain\":%s,", re[nColumn+9]);
		sJsonData.appendWithFormat("\"Money\":%s,", re[nColumn+10]);
		sJsonData.appendWithFormat("\"Version\":%s,", re[nColumn+11]);
		sJsonData.appendWithFormat("\"UserToken\":\"%s\",", re[nColumn+12]);

		std::string sDeviceId = "windows";
		std::string sDeviceName = "PC";
		std::string sOSVersion = "windows 8";

		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			sDeviceId = FacebookManager::getInstance()->getDeviceId();
			sDeviceName = FacebookManager::getInstance()->getDeviceName();
			sOSVersion = FacebookManager::getInstance()->getOSVersion();
		#endif

		sJsonData.appendWithFormat("\"DeviceId\":\"%s\",", sDeviceId.c_str());
		sJsonData.appendWithFormat("\"DeviceName\":\"%s\",", sDeviceName.c_str());
		sJsonData.appendWithFormat("\"OSVersion\":\"%s\"", sOSVersion.c_str());
	}
	sJsonData.append("}");
	sqlite3_free_table(re);

	return sJsonData.getCString();
}

bool UserTable::updateDataSyncUser(cs::JsonDictionary* pJsonSync, const int& iVersion)
{
	cs::JsonDictionary *pJsonUser = pJsonSync->getSubDictionary("User");
	if (pJsonUser != NULL)
	{
		String sql = "update Users Set";

		std::string sFacebookId = pJsonUser->getItemStringValue("FacebookId");
		sFacebookId.pop_back();

		sql.appendWithFormat(" FacebookId='%s',", sFacebookId.c_str());
		sql.appendWithFormat(" FacebookToken='%s',", pJsonUser->getItemStringValue("FacebookToken"));
		sql.appendWithFormat(" FirstName='%s',", pJsonUser->getItemStringValue("FirstName"));
		sql.appendWithFormat(" LastName='%s',", pJsonUser->getItemStringValue("LastName"));
		sql.appendWithFormat(" CurrentChapter='%s',", pJsonUser->getItemStringValue("CurrentChapter"));
		sql.appendWithFormat(" CurrentLevel=%d,", pJsonUser->getItemIntValue("CurrentLevel", 1));
		//sql.appendWithFormat(" Life=%d,", pJsonUser->getItemIntValue("Life", 0));
		//sql.appendWithFormat(" LifeTimeRemaining=%d,", pJsonUser->getItemIntValue("LifeTimeRemaining", 0));
		//sql.appendWithFormat(" LifeTimeBeginRemain=%u,", getTimeLocalCurrent());
		sql.appendWithFormat(" Monney=%d,", pJsonUser->getItemIntValue("Money", 0));
		sql.appendWithFormat(" Version=%d,", iVersion);
		sql.appendWithFormat(" UserIdentifier=%d,", pJsonUser->getItemIntValue("UserId", 0));
		sql.appendWithFormat(" UserToken='%s'", pJsonUser->getItemStringValue("UserToken"));

		int iResult = sqlite3_exec(InitDatabase::getInstance()->getDatabseSqlite(), sql.getCString(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK)
			return false;
	}

	CCLOG("sync User true");
	this->fetchhUser();
	return true;
}

void UserTable::haveGenerateNextChapter()
{
	ChapterInfo chapterInfo = ChapterTable::getInstance()->getChapterInfo(m_userInfo.sCurrentChapterId);
	if (chapterInfo.sChapterId != "" && chapterInfo.bIsUnlock)
	{
		ChapterConfig chapterConfig;
        GameConfigManager::getInstance()->GetChapterConfigDirectly(m_userInfo.sCurrentChapterId, &chapterConfig);
		if (chapterConfig.m_iTotalevel <= m_userInfo.iCurrentLevel)
		{
			LevelInfo levelInfo = LevelTable::getInstance()->getLevel(m_userInfo.sCurrentChapterId, chapterConfig.m_iTotalevel);
			if (levelInfo.bIsUnlock == true)
			{
				std::string sNextChapterID;
				if ( GameConfigManager::getInstance()->GetNextChapterID(m_userInfo.sCurrentChapterId, sNextChapterID))
				{
					InitDatabase::getInstance()->createDataChapterAndLevel(sNextChapterID);
					m_userInfo.sCurrentChapterId = sNextChapterID;
					m_userInfo.iCurrentLevel = 1;

					updateUser(m_userInfo);
				}
			}
		}
	}
}
