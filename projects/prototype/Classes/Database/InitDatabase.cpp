#include "InitDatabase.h"

USING_NS_CC; 

InitDatabase* InitDatabase::m_InitDatabase = NULL;

InitDatabase::InitDatabase()
: m_DatabaseSqlite(NULL)
{

}

void InitDatabase::releaseInstance()
{
	if (m_InitDatabase == NULL)
	{
		delete m_InitDatabase;
	}

	sqlite3_close(m_InitDatabase->m_DatabaseSqlite);
	m_InitDatabase->m_DatabaseSqlite = NULL;
	m_InitDatabase = NULL;	
}

InitDatabase* InitDatabase::getInstance()
{
	if (m_InitDatabase == NULL) {
		m_InitDatabase = new InitDatabase();
		if (!m_InitDatabase->init())
		{
			Director::getInstance()->end();
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
			#endif
		}
	}

	return m_InitDatabase;
}

bool InitDatabase::init()
{
	m_spath = FileUtils::sharedFileUtils()->getWritablePath() + "ohmyword.db3";
	
	if(!FileUtils::getInstance()->isFileExist(m_spath))
	{
		UserDefault::getInstance()->setIntegerForKey("InitDatabase", 0);
	}
	else if (UserDefault::getInstance()->getIntegerForKey("InitDatabase", 0) == 0)
	{
		remove(m_spath.c_str());
	}
 
	int iResult;  
	iResult = sqlite3_open(m_spath.c_str(),&m_DatabaseSqlite);  
	if(iResult != SQLITE_OK)
	{
		sqlite3_close(m_DatabaseSqlite);
		m_DatabaseSqlite = NULL;
		remove(m_spath.c_str());
		return false;
	}

	if (UserDefault::getInstance()->getIntegerForKey("InitDatabase", 0) == 0)
	{
		if (createDatabase())
		{
			UserDefault::getInstance()->setIntegerForKey("InitDatabase", 1);
		}
		else
		{
			sqlite3_close(m_DatabaseSqlite);
			m_DatabaseSqlite = NULL;
			remove(m_spath.c_str());
			return false;
		}
	}

	return true;
}

bool InitDatabase::createDatabase()
{
	bool bCreateSuccess = true;
	int iResult;
	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Users(UserIdentifier text primary key, UserId integer, FacebookId text, FacebookToken text, FirstName text, LastName text, CurrentChapter integer, CurrentLevel integer, Life integer, LifeTimeRemaining integer, LifeTimeBeginRemain integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
	{
		bCreateSuccess = false;
	}
	else 
	{
		std::string sql;
		sql="insert into Users values('vandao', 0, '', '', '', '', 1, 1, 5, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;
	}

	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Chapters(Chapter integer primary key, Name text, MinLevel integer, MaxLevel integer, TotalLevelUnlock integer, TotalStar integer, TotalScore integer, IsUnlock integer, IsUpdate integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
	{
		bCreateSuccess = false;
	}
	else
	{
		std::string sql;
		sql="insert into Chapters values(1, 'School', 1, 20, 0, 0, 0, 1, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;
		
		sql="insert into Chapters values(2, 'House', 21, 40, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

		sql="insert into Chapters values(3, 'Office', 41, 60, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;
		
		sql="insert into Chapters values(4, 'The Zoo', 61, 80, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

		sql="insert into Chapters values(5, 'The Zoo', 81, 100, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

		sql="insert into Chapters values(6, 'Office', 101, 120, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

		sql="insert into Chapters values(7, 'School', 121, 140, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;
		
		sql="insert into Chapters values(8, 'House', 141, 160, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

		sql="insert into Chapters values(9, 'Office', 161, 180, 0, 0, 0, 0, 0)";
		iResult = sqlite3_exec(m_DatabaseSqlite, sql.c_str(), NULL, NULL, NULL);
		if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
			bCreateSuccess = false;

	}

	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Levels(Level integer primary key, Chapter integer, Star integer, Score integer, BonusQuest integer, IsUnlock integer, IsUpdate integer, WordKey text)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
	{
		bCreateSuccess = false;
	}
	else
	{
		for (int iIndex=0; iIndex<40; iIndex++)
		{
			int iChapter = iIndex/20 + 1;
			int iLevel = iIndex+1;

			String sql = "insert into Levels values(";
			sql.appendWithFormat("%d,", iLevel);
			sql.appendWithFormat("%d,", iChapter);
			sql.append("0,0,0,0,0,'')");
			CCLOG("%s", sql.getCString());

			iResult = sqlite3_exec(m_DatabaseSqlite, sql.getCString(), NULL, NULL, NULL);
			if(iResult != SQLITE_OK && iResult != SQLITE_CONSTRAINT)
				bCreateSuccess = false;
		}
	}

	iResult = sqlite3_exec(m_DatabaseSqlite,"create table if not exists Friends(FriendId integer primary key, FacebookId text, FirstName text, LastName text, CurrentChapter integer, CurrentLevel integer)", NULL, NULL, NULL);
	if(iResult != SQLITE_OK)
		bCreateSuccess = false;

	return bCreateSuccess;
}
